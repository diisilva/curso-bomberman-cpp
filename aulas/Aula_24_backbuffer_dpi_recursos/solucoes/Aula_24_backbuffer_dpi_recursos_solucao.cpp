// Aula 24 — solução do exercício 2: clicar no cartão certo em qualquer tamanho.
//
// O mouse informa a posição em pixels da JANELA. O desenho está deslocado pela
// margem e multiplicado pela escala. Para saber em que cartão o clique caiu,
// faça a conta ao contrário, como o WM_LBUTTONUP do jogo:
//     x_jogo = (x_janela - margemJanelaX) / escala
//
// Três peças do jogo real (projeto_final/bomberman.cpp):
//   garantirBuffer / liberarBuffer → um bitmap fora da tela que só é recriado
//                                    quando o tamanho da janela muda
//   ajustarAreaDesenho + px()      → o jogo é desenhado num tamanho fixo e
//                                    escalado para a janela, sem distorcer
//   WM_DPICHANGED                  → a janela acompanha a escala do monitor
// Redimensione a janela e arraste-a entre monitores com escalas diferentes.
#include <windows.h>
#include <gdiplus.h>
#include <algorithm>
#include <string>

ULONG_PTR tokenGdiPlus = 0;
const int LARGURA = 560, ALTURA = 420;   // no jogo: LARGURA_JOGO x ALTURA_JOGO (1080 x 852)

float escala = 1.0f;                      // no jogo: escalaDpi
int margemJanelaX = 0, margemJanelaY = 0;
int px(int valor) { return static_cast<int>(valor * escala + 0.5f); }

// ---------- backbuffer ----------
HDC bufferDC = nullptr;
HBITMAP bufferBitmap = nullptr;
int bufferLargura = 0, bufferAltura = 0;
int buffersCriados = 0;                   // só para ver na tela quantas vezes foi recriado
int selecionado = -1;                     // cartão clicado (-1 = nenhum)

void liberarBuffer() {
    if (bufferDC) { DeleteDC(bufferDC); bufferDC = nullptr; }
    if (bufferBitmap) { DeleteObject(bufferBitmap); bufferBitmap = nullptr; }
    bufferLargura = bufferAltura = 0;
}
void garantirBuffer(HDC referencia, int largura, int altura) {
    if (bufferDC && bufferLargura == largura && bufferAltura == altura) return;   // reaproveita
    liberarBuffer();
    bufferDC = CreateCompatibleDC(referencia);
    bufferBitmap = CreateCompatibleBitmap(referencia, largura, altura);
    SelectObject(bufferDC, bufferBitmap);
    bufferLargura = largura;
    bufferAltura = altura;
    buffersCriados++;
}

// A maior escala que cabe nos dois eixos; a sobra vira faixa nas laterais.
void ajustarAreaDesenho(int largura, int altura) {
    if (largura <= 0 || altura <= 0) return;
    escala = std::min(static_cast<float>(largura) / LARGURA, static_cast<float>(altura) / ALTURA);
    margemJanelaX = (largura - px(LARGURA)) / 2;
    margemJanelaY = (altura - px(ALTURA)) / 2;
}

// ---------- desenho (as mesmas funções da Aula 04, agora com px) ----------
void painel(Gdiplus::Graphics& g, int x, int y, int largura, int altura,
            Gdiplus::Color cor = Gdiplus::Color(255, 25, 34, 50)) {
    Gdiplus::SolidBrush pincel(cor);
    g.FillRectangle(&pincel, px(x), px(y), px(largura), px(altura));
}
void desenharTexto(Gdiplus::Graphics& g, const std::wstring& texto, int x, int y, int tamanho,
                   Gdiplus::Color cor = Gdiplus::Color(255, 220, 230, 240)) {
    Gdiplus::Font fonte(L"Segoe UI", static_cast<Gdiplus::REAL>(px(tamanho)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(texto.c_str(), -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(x)), Gdiplus::REAL(px(y))), &pincel);
}

void desenharCena(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 11, 16, 28));
    painel(g, 0, 0, LARGURA, 5, Gdiplus::Color(255, 252, 187, 65));
    desenharTexto(g, L"BOMBERMAN", 30, 24, 40, Gdiplus::Color(255, 246, 247, 251));
    const wchar_t* titulos[] = {L"1   JOGAR", L"2   DEMONSTRACAO", L"3   COMO JOGAR"};
    for (int i = 0; i < 3; ++i) {
        int y = 100 + i * 57;
        painel(g, 30, y, 300, 47);
        Gdiplus::Color tarja = i == selecionado ? Gdiplus::Color(255, 98, 235, 199) : Gdiplus::Color(255, 110, 157, 186);
        painel(g, 30, y, 3, 47, tarja);
        desenharTexto(g, titulos[i], 48, y + 10, 18, i == selecionado ? tarja : Gdiplus::Color(255, 220, 230, 240));
    }
    wchar_t info[128];
    swprintf(info, 128, L"escala=%.2f  margem=(%d,%d)  buffers criados=%d", escala, margemJanelaX, margemJanelaY, buffersCriados);
    desenharTexto(g, info, 30, 300, 14, Gdiplus::Color(255, 98, 235, 199));
    desenharTexto(g, L"Clique num cartao; depois redimensione e clique de novo.", 30, 330, 14, Gdiplus::Color(255, 144, 164, 190));
}

void desenhar(HWND janela) {
    PAINTSTRUCT ps;
    HDC destino = BeginPaint(janela, &ps);
    RECT cliente;
    GetClientRect(janela, &cliente);
    garantirBuffer(destino, cliente.right, cliente.bottom);
    {
        Gdiplus::Graphics g(bufferDC);
        Gdiplus::SolidBrush faixa(Gdiplus::Color(255, 0, 0, 0));    // fundo das faixas laterais
        g.FillRectangle(&faixa, 0, 0, cliente.right, cliente.bottom);
        g.TranslateTransform(static_cast<float>(margemJanelaX), static_cast<float>(margemJanelaY));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Region area(Gdiplus::Rect(0, 0, px(LARGURA), px(ALTURA)));
        g.SetClip(&area);
        desenharCena(g);
    }
    // Um único BitBlt copia o quadro pronto: a tela nunca mostra o desenho pela metade.
    BitBlt(destino, 0, 0, cliente.right, cliente.bottom, bufferDC, 0, 0, SRCCOPY);
    EndPaint(janela, &ps);
}

LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_PAINT:
        desenhar(janela);
        return 0;
    case WM_ERASEBKGND:
        return 1;   // "já apaguei": sem isso o Windows pinta o fundo antes e a tela pisca
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) {
            ajustarAreaDesenho(LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(janela, nullptr, FALSE);
        }
        return 0;
    case WM_DPICHANGED: {
        // O Windows sugere um retângulo novo, já na escala do monitor de destino.
        RECT* sugerido = reinterpret_cast<RECT*>(lParam);
        SetWindowPos(janela, nullptr, sugerido->left, sugerido->top, sugerido->right - sugerido->left,
                     sugerido->bottom - sugerido->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }
    case WM_LBUTTONUP: {
        // short: LOWORD pode ser "negativo" num segundo monitor à esquerda.
        int x = static_cast<int>((static_cast<short>(LOWORD(lParam)) - margemJanelaX) / escala);
        int y = static_cast<int>((static_cast<short>(HIWORD(lParam)) - margemJanelaY) / escala);
        selecionado = -1;
        for (int i = 0; i < 3; ++i)
            if (x >= 30 && x < 330 && y >= 100 + i * 57 && y < 147 + i * 57) selecionado = i;
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) DestroyWindow(janela);
        return 0;
    case WM_DESTROY:
        liberarBuffer();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    // Sem isso, o Windows desenha a janela em 96 DPI e amplia como imagem: tudo fica borrado.
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;
    classe.hInstance = instancia;
    classe.lpszClassName = L"Aula24Buffer";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 2; }
    float escalaMonitor = GetDpiForSystem() / 96.0f;     // 1,0 a 100%; 1,5 a 150%
    RECT area{0, 0, static_cast<LONG>(LARGURA * escalaMonitor), static_cast<LONG>(ALTURA * escalaMonitor)};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 24 - backbuffer e DPI", WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, area.right - area.left, area.bottom - area.top,
                                  nullptr, nullptr, instancia, nullptr);
    if (!janela) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 3; }
    ShowWindow(janela, exibir);
    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) {
        TranslateMessage(&evento);
        DispatchMessageW(&evento);
    }
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    return static_cast<int>(evento.wParam);
}
