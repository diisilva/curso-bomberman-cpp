// Aula 04 — solução do exercício 2: o destaque segue as setas.
//
// O desenho continua só LENDO o estado (a variável selecionado). Quem muda o
// estado é WM_KEYDOWN, que depois pede um novo WM_PAINT com InvalidateRect.
// É a mesma divisão do menu real: lerTeclado altera, desenharMenu mostra.
#include <windows.h>
#include <gdiplus.h>
#include <string>

ULONG_PTR tokenGdiPlus = 0;
const int LARGURA = 560, ALTURA = 420;
int selecionado = 0;   // estado: qual cartão está em destaque (0 a 2)

// Retângulo cheio. Color(alfa, vermelho, verde, azul); alfa 255 = opaco.
void painel(Gdiplus::Graphics& g, int x, int y, int largura, int altura,
            Gdiplus::Color cor = Gdiplus::Color(255, 25, 34, 50)) {
    Gdiplus::SolidBrush pincel(cor);
    g.FillRectangle(&pincel, x, y, largura, altura);
}

// No jogo a fonte vem de um cache (obterFonte); criar uma Font por chamada
// funciona, mas custa caro quando o menu tem dezenas de textos por quadro.
void desenharTexto(Gdiplus::Graphics& g, const std::wstring& texto, float x, float y,
                   float tamanho, Gdiplus::Color cor = Gdiplus::Color(255, 220, 230, 240)) {
    Gdiplus::Font fonte(L"Segoe UI", tamanho, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(texto.c_str(), -1, &fonte, Gdiplus::PointF(x, y), &pincel);
}

// Fundo de todas as telas: azul quase preto e a faixa dourada no topo.
void fundoInterface(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 11, 16, 28));
    painel(g, 0, 0, LARGURA, 5, Gdiplus::Color(255, 252, 187, 65));
}

void desenharCena(Gdiplus::Graphics& g) {
    fundoInterface(g);
    desenharTexto(g, L"BOMBERMAN", 30, 24, 40, Gdiplus::Color(255, 246, 247, 251));

    // Três cartões como os de desenharMenu: o selecionado fica em verde.
    const wchar_t* titulos[] = {L"1   JOGAR", L"2   DEMONSTRACAO", L"3   COMO JOGAR"};
    for (int i = 0; i < 3; ++i) {
        int y = 100 + i * 57;
        Gdiplus::Color tarja = i == selecionado ? Gdiplus::Color(255, 98, 235, 199) : Gdiplus::Color(255, 110, 157, 186);
        painel(g, 30, y, 300, 47);          // corpo do cartão
        painel(g, 30, y, 3, 47, tarja);     // tarja de 3 px à esquerda
        desenharTexto(g, titulos[i], 48, y + 10, 18, i == selecionado ? tarja : Gdiplus::Color(255, 220, 230, 240));
    }

    // Transparência: a sombra do personagem é preta com alfa 105 (de 255).
    // Ela escurece o chão sem escondê-lo.
    painel(g, 370, 100, 160, 160, Gdiplus::Color(255, 0, 140, 200));    // "chão" azul
    Gdiplus::SolidBrush sombra(Gdiplus::Color(105, 0, 0, 0));
    g.FillEllipse(&sombra, 400, 215, 100, 30);
    Gdiplus::SolidBrush corpo(Gdiplus::Color(255, 246, 247, 251));
    g.FillEllipse(&corpo, 420, 130, 60, 95);                            // "personagem"
    desenharTexto(g, L"sombra: Color(105, 0, 0, 0)", 370, 268, 13, Gdiplus::Color(255, 144, 164, 190));

    desenharTexto(g, L"SETAS   escolher     Q / ESC   sair", 30, 370, 14);
}

LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_PAINT: {
        // Só desenhe dentro de BeginPaint/EndPaint: o par informa ao Windows
        // que a área foi pintada e libera o HDC.
        PAINTSTRUCT pintura{};
        HDC dc = BeginPaint(janela, &pintura);
        {
            Gdiplus::Graphics g(dc);   // o Graphics precisa morrer antes de EndPaint
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            desenharCena(g);
        }
        EndPaint(janela, &pintura);
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == 'Q' || wParam == VK_ESCAPE) { DestroyWindow(janela); return 0; }
        // Circular, como o seletor de fases: de 2 para baixo volta a 0.
        if (wParam == VK_DOWN) selecionado = (selecionado + 1) % 3;
        if (wParam == VK_UP) selecionado = (selecionado + 2) % 3;
        InvalidateRect(janela, nullptr, FALSE);   // pede um WM_PAINT novo
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    // GDI+ liga antes da primeira janela e desliga depois do laço, como no jogo.
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;
    classe.hInstance = instancia;
    classe.lpszClassName = L"AulaFormas";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 2; }
    RECT area{0, 0, LARGURA, ALTURA};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 04 - GDI+", WS_OVERLAPPEDWINDOW,
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
    return 0;
}
