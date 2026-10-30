// Aula 30 — solução do exercício 2: uma tela nova de OPÇÕES.
//
// Para acrescentar uma tela, quatro lugares mudam, e só eles:
//   1. o enum TelaMenu ganha MENU_OPCOES;
//   2. lerTeclado ganha um bloco para a tela (e o atalho O no principal);
//   3. clicar ganha a área do cartão;
//   4. desenharCena ganha o desenho da tela.
// O joystick funciona sem nenhuma linha nova: ele já chama lerTeclado.
//
// Um menu pequeno com as mesmas técnicas do menu real (projeto_final/bomberman.cpp):
//   - estado explícito: enum TelaMenu + telaAposNomes (para onde voltar depois dos nomes)
//   - entrada muda o estado; desenhar só lê (lerTeclado × desenharCena)
//   - campo de texto: WM_CHAR digita, WM_KEYDOWN move o cursor; repetição do SO filtrada
//   - mouse: o clique é convertido de pixels da janela para coordenadas do menu
//   - joystick XInput: imita o teclado chamando lerTeclado, só no TOQUE do botão
// Telas: principal → nome → dificuldade → fases → partida (ou 8 direto para fases).
#include <windows.h>
#include <gdiplus.h>
#include <xinput.h>
#include <algorithm>
#include <cstring>
#include <string>

ULONG_PTR tokenGdiPlus = 0;
const int LARGURA = 760, ALTURA = 520;    // coordenadas de projeto do menu
const int TOTAL_FASES = 10;

// ---------- estado ----------
enum TelaMenu { MENU_PRINCIPAL, MENU_NOMES, MENU_DIFICULDADE, MENU_FASES, PARTIDA, MENU_OPCOES };
TelaMenu telaMenu = MENU_PRINCIPAL;
TelaMenu telaAposNomes = MENU_DIFICULDADE;
std::wstring nomeJogador, nomeConfirmado, avisoNome;
size_t cursorNome = 0;
int dificuldade = 0, faseSelecionada = 1;
bool musicaLigada = true;   // NOVO: a opção da tela nova
std::wstring ultimoEvento = L"(nenhum)";
ULONGLONG relogio = 0;

// ---------- escala (Aula 24) ----------
float escala = 1.0f;
int margemX = 0, margemY = 0;
int px(int v) { return static_cast<int>(v * escala + 0.5f); }
void ajustarAreaDesenho(int largura, int altura) {
    if (largura <= 0 || altura <= 0) return;
    escala = std::min(float(largura) / LARGURA, float(altura) / ALTURA);
    margemX = (largura - px(LARGURA)) / 2;
    margemY = (altura - px(ALTURA)) / 2;
}

// ---------- nomes ----------
bool nomesProntos() { return !nomeConfirmado.empty() && nomeConfirmado == nomeJogador; }
void abrirTelaNomes(TelaMenu depois) {
    telaAposNomes = depois;
    telaMenu = MENU_NOMES;
    cursorNome = nomeJogador.size();
    avisoNome.clear();
}
// Como digitarNomeJogador: chega por WM_CHAR, já com acento e maiúscula resolvidos.
void digitarNomeJogador(wchar_t caractere) {
    if (telaMenu != MENU_NOMES) return;
    cursorNome = std::min(cursorNome, nomeJogador.size());
    if (caractere == L'\b') {                              // Backspace também chega como caractere
        if (cursorNome > 0) nomeJogador.erase(--cursorNome, 1);
        return;
    }
    if (caractere >= L' ' && caractere != 127 && nomeJogador.size() < 24) {
        nomeJogador.insert(cursorNome, 1, caractere);
        cursorNome++;
        avisoNome.clear();
    }
}
void confirmarCampoNome() {
    // O jogo também limpa espaços repetidos e confere se o nome já está no ranking.
    if (nomeJogador.find_first_not_of(L' ') == std::wstring::npos) { avisoNome = L"Digite o nome antes de jogar."; return; }
    nomeConfirmado = nomeJogador;
    telaMenu = telaAposNomes;
}

// ---------- teclado: o único lugar que muda telaMenu ----------
void lerTeclado(WPARAM tecla, LPARAM lParam) {
    if (lParam & (1 << 30)) return;   // bit 30: repetição automática do Windows (segurar a tecla)
    if (telaMenu == MENU_NOMES) {
        if (tecla == VK_RETURN) confirmarCampoNome();
        else if (tecla == VK_ESCAPE) telaMenu = MENU_PRINCIPAL;
        else if (tecla == VK_LEFT && cursorNome > 0) cursorNome--;
        else if (tecla == VK_RIGHT) cursorNome = std::min(cursorNome + 1, nomeJogador.size());
        else if (tecla == VK_HOME) cursorNome = 0;
        else if (tecla == VK_END) cursorNome = nomeJogador.size();
        else if (tecla == VK_DELETE && cursorNome < nomeJogador.size()) nomeJogador.erase(cursorNome, 1);
        return;   // letras NÃO são tratadas aqui: chegam depois, por WM_CHAR
    }
    if (telaMenu == MENU_DIFICULDADE) {
        if (tecla >= '1' && tecla <= '3') { dificuldade = int(tecla - '1'); telaMenu = MENU_FASES; }
        else if (tecla == VK_LEFT) dificuldade = (dificuldade + 2) % 3;
        else if (tecla == VK_RIGHT) dificuldade = (dificuldade + 1) % 3;
        else if (tecla == VK_RETURN) telaMenu = MENU_FASES;
        else if (tecla == 'M') abrirTelaNomes(MENU_DIFICULDADE);
        else if (tecla == VK_ESCAPE) telaMenu = MENU_PRINCIPAL;
        return;
    }
    if (telaMenu == MENU_FASES) {
        int escolha = tecla >= '1' && tecla <= '9' ? int(tecla - '0') : tecla == '0' ? 10 : 0;
        if (escolha) faseSelecionada = escolha;
        else if (tecla == VK_LEFT) faseSelecionada = (faseSelecionada + TOTAL_FASES - 2) % TOTAL_FASES + 1;
        else if (tecla == VK_RIGHT) faseSelecionada = faseSelecionada % TOTAL_FASES + 1;
        else if (tecla == VK_UP || tecla == VK_DOWN) faseSelecionada = (faseSelecionada + 4) % TOTAL_FASES + 1;
        else if (tecla == VK_RETURN) { if (nomesProntos()) telaMenu = PARTIDA; else abrirTelaNomes(MENU_FASES); }
        else if (tecla == 'M') telaMenu = MENU_DIFICULDADE;
        else if (tecla == VK_ESCAPE) telaMenu = MENU_PRINCIPAL;
        return;
    }
    if (telaMenu == MENU_OPCOES) {          // NOVO
        if (tecla == VK_RETURN || tecla == VK_LEFT || tecla == VK_RIGHT) musicaLigada = !musicaLigada;
        else if (tecla == 'M' || tecla == VK_ESCAPE) telaMenu = MENU_PRINCIPAL;
        return;
    }
    if (telaMenu == PARTIDA) {
        if (tecla == 'M' || tecla == VK_ESCAPE) telaMenu = MENU_PRINCIPAL;
        return;
    }
    if (tecla == '1') abrirTelaNomes(MENU_DIFICULDADE);
    else if (tecla == '8') telaMenu = MENU_FASES;          // atalho: a guarda dos nomes fica no Enter
    else if (tecla == 'O' || tecla == VK_DOWN) telaMenu = MENU_OPCOES;   // NOVO (baixo: para o direcional do controle)
}

// ---------- mouse: só a tela de dificuldade tem cartões clicáveis aqui ----------
void clicar(int xJanela, int yJanela) {
    int x = static_cast<int>((xJanela - margemX) / escala);   // mesma conta do WM_LBUTTONUP real
    int y = static_cast<int>((yJanela - margemY) / escala);
    if (telaMenu == MENU_PRINCIPAL && x >= 30 && x < 370 && y >= 288 && y < 348) lerTeclado('O', 0);   // NOVO
    if (telaMenu == MENU_OPCOES && x >= 30 && x < 370 && y >= 140 && y < 200) lerTeclado(VK_RETURN, 0); // NOVO
    if (telaMenu == MENU_DIFICULDADE && y >= 150 && y < 330) {
        int coluna = (x - 40) / 230;
        if (x >= 40 && coluna < 3 && (x - 40) % 230 < 210) lerTeclado('1' + coluna, 0);
    }
}

// ---------- joystick: imita o teclado (processarControleNoMenu) ----------
using FuncaoXInputGetState = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
HMODULE bibliotecaXInput = nullptr;
FuncaoXInputGetState obterEstadoXInput = nullptr;
WORD botoesAnteriores = 0;
int direcaoAnterior = 0;
bool novo(bool agora, bool antes) { return agora && !antes; }

void processarControleNoMenu() {
    if (!obterEstadoXInput) return;
    XINPUT_STATE estado{};
    if (obterEstadoXInput(0, &estado) != ERROR_SUCCESS) { botoesAnteriores = 0; direcaoAnterior = 0; return; }
    WORD b = estado.Gamepad.wButtons;
    int direcao = (b & XINPUT_GAMEPAD_DPAD_UP) ? VK_UP : (b & XINPUT_GAMEPAD_DPAD_DOWN) ? VK_DOWN
                : (b & XINPUT_GAMEPAD_DPAD_LEFT) ? VK_LEFT : (b & XINPUT_GAMEPAD_DPAD_RIGHT) ? VK_RIGHT : 0;
    if (direcao && direcao != direcaoAnterior) { ultimoEvento = L"joystick: direcional"; lerTeclado(direcao, 0); }
    else if (novo(b & XINPUT_GAMEPAD_A, botoesAnteriores & XINPUT_GAMEPAD_A)) {
        ultimoEvento = L"joystick: A";
        lerTeclado(telaMenu == MENU_PRINCIPAL ? '1' : VK_RETURN, 0);
    } else if (novo(b & XINPUT_GAMEPAD_B, botoesAnteriores & XINPUT_GAMEPAD_B)) {
        ultimoEvento = L"joystick: B";
        lerTeclado(telaMenu == MENU_NOMES ? VK_ESCAPE : 'M', 0);
    }
    botoesAnteriores = b;
    direcaoAnterior = direcao;
}

// ---------- desenho: só lê o estado ----------
void painel(Gdiplus::Graphics& g, int x, int y, int l, int a, Gdiplus::Color cor = Gdiplus::Color(255, 25, 34, 50)) {
    Gdiplus::SolidBrush pincel(cor);
    g.FillRectangle(&pincel, px(x), px(y), px(l), px(a));
}
void desenharTexto(Gdiplus::Graphics& g, const std::wstring& texto, int x, int y, int tamanho,
                   Gdiplus::Color cor = Gdiplus::Color(255, 220, 230, 240)) {
    Gdiplus::Font fonte(L"Segoe UI", Gdiplus::REAL(px(tamanho)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(texto.c_str(), -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(x)), Gdiplus::REAL(px(y))), &pincel);
}
const Gdiplus::Color VERDE(255, 98, 235, 199), AZUL(255, 110, 157, 186), DOURADO(255, 252, 187, 65), CINZA(255, 144, 164, 190);

void cartao(Gdiplus::Graphics& g, int x, int y, int l, int a, const std::wstring& titulo, bool marcado) {
    painel(g, x, y, l, a);
    painel(g, x, y, 4, a, marcado ? VERDE : AZUL);
    desenharTexto(g, titulo, x + 18, y + a / 2 - 14, 20, marcado ? VERDE : Gdiplus::Color(255, 220, 230, 240));
}

void desenharCena(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 11, 16, 28));
    painel(g, 0, 0, LARGURA, 5, DOURADO);
    const wchar_t* nomesTelas[] = {L"PRINCIPAL", L"NOMES", L"DIFICULDADE", L"FASES", L"PARTIDA", L"OPCOES"};
    desenharTexto(g, std::wstring(L"tela: ") + nomesTelas[telaMenu], 30, 20, 14, CINZA);
    const wchar_t* dificuldades[] = {L"FACIL", L"MEDIO", L"DIFICIL"};
    if (telaMenu == MENU_PRINCIPAL) {
        desenharTexto(g, L"BOMBERMAN", 30, 50, 40, Gdiplus::Color(255, 246, 247, 251));
        cartao(g, 30, 140, 340, 60, L"1   JOGAR", true);
        cartao(g, 30, 214, 340, 60, L"8   SELETOR DE FASES", false);
        cartao(g, 30, 288, 340, 60, L"O   OPCOES", false);   // NOVO
        desenharTexto(g, nomeJogador.empty() ? L"(sem nome)" : L"jogador: " + nomeJogador, 400, 160, 16, CINZA);
    } else if (telaMenu == MENU_NOMES) {
        desenharTexto(g, L"NOME PARA O RANKING", 30, 50, 32, Gdiplus::Color(255, 246, 247, 251));
        painel(g, 30, 130, 500, 70, Gdiplus::Color(255, 30, 52, 66));
        painel(g, 30, 130, 5, 70, VERDE);
        // Como desenharNomeDigitavel: texto e medida com o MESMO formato tipográfico,
        // senão a margem interna do formato padrão desloca o cursor.
        Gdiplus::Font fonte(L"Segoe UI", Gdiplus::REAL(px(22)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat formato(Gdiplus::StringFormat::GenericTypographic());
        formato.SetFormatFlags(formato.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
        Gdiplus::SolidBrush corNome(nomeJogador.empty() ? CINZA : Gdiplus::Color(255, 246, 247, 251));
        const wchar_t* textoNome = nomeJogador.empty() ? L"Digite seu nome" : nomeJogador.c_str();
        g.DrawString(textoNome, -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(50)), Gdiplus::REAL(px(150))), &formato, &corNome);
        if ((relogio / 500) % 2 == 0) {   // cursor piscando a cada meio segundo
            Gdiplus::RectF medida;
            std::wstring prefixo = nomeJogador.substr(0, cursorNome);
            g.MeasureString(prefixo.c_str(), int(prefixo.size()), &fonte, Gdiplus::PointF(0, 0), &formato, &medida);
            Gdiplus::Pen caneta(VERDE, Gdiplus::REAL(std::max(1, px(2))));
            float cx = px(50) + medida.Width + 1;
            g.DrawLine(&caneta, cx, Gdiplus::REAL(px(151)), cx, Gdiplus::REAL(px(179)));
        }
        desenharTexto(g, avisoNome, 30, 215, 16, Gdiplus::Color(255, 255, 140, 110));
        desenharTexto(g, L"ENTER confirma   ESC volta   SETAS/HOME/END movem o cursor", 30, 250, 14, CINZA);
        desenharTexto(g, std::wstring(L"depois dos nomes vai para: ") + nomesTelas[telaAposNomes], 30, 275, 14, CINZA);
    } else if (telaMenu == MENU_DIFICULDADE) {
        desenharTexto(g, L"DIFICULDADE", 30, 50, 32, Gdiplus::Color(255, 246, 247, 251));
        for (int i = 0; i < 3; ++i) cartao(g, 40 + i * 230, 150, 210, 180, std::to_wstring(i + 1) + L"  " + dificuldades[i], i == dificuldade);
        desenharTexto(g, L"1/2/3, SETAS + ENTER ou CLIQUE no cartao   M nomes   ESC menu", 30, 350, 14, CINZA);
    } else if (telaMenu == MENU_FASES) {
        desenharTexto(g, L"FASES", 30, 50, 32, Gdiplus::Color(255, 246, 247, 251));
        for (int i = 0; i < TOTAL_FASES; ++i) {
            int x = 30 + (i % 5) * 140, y = 120 + (i / 5) * 110;
            cartao(g, x, y, 125, 95, std::to_wstring(i + 1), i + 1 == faseSelecionada);
        }
        desenharTexto(g, L"1-9 e 0 (=10), SETAS circulares, ENTER joga   M volta", 30, 350, 14, CINZA);
    } else if (telaMenu == MENU_OPCOES) {   // NOVO
        desenharTexto(g, L"OPCOES", 30, 50, 32, Gdiplus::Color(255, 246, 247, 251));
        cartao(g, 30, 140, 340, 60, musicaLigada ? L"MUSICA: LIGADA" : L"MUSICA: DESLIGADA", musicaLigada);
        desenharTexto(g, L"ENTER, SETAS ou CLIQUE alternam   ESC volta", 30, 220, 14, CINZA);
    } else {
        desenharTexto(g, L"A PARTIDA COMECARIA AGORA", 30, 50, 32, VERDE);
        desenharTexto(g, L"jogador: " + nomeJogador, 30, 120, 20);
        desenharTexto(g, std::wstring(L"dificuldade: ") + dificuldades[dificuldade], 30, 155, 20);
        desenharTexto(g, L"fase: " + std::to_wstring(faseSelecionada), 30, 190, 20);
        desenharTexto(g, L"M volta ao menu", 30, 240, 14, CINZA);
    }
    painel(g, 0, 470, LARGURA, 50, Gdiplus::Color(255, 18, 27, 43));
    desenharTexto(g, L"ultimo evento: " + ultimoEvento, 30, 484, 14, DOURADO);
    desenharTexto(g, obterEstadoXInput ? L"XInput pronto (controle 0)" : L"sem XInput", 520, 484, 14, CINZA);
}

// ---------- janela ----------
HDC bufferDC = nullptr; HBITMAP bufferBitmap = nullptr; int bufferL = 0, bufferA = 0;
void desenhar(HWND janela) {
    PAINTSTRUCT ps;
    HDC destino = BeginPaint(janela, &ps);
    RECT r; GetClientRect(janela, &r);
    if (!bufferDC || bufferL != r.right || bufferA != r.bottom) {   // garantirBuffer da Aula 24
        if (bufferDC) { DeleteDC(bufferDC); DeleteObject(bufferBitmap); }
        bufferDC = CreateCompatibleDC(destino);
        bufferBitmap = CreateCompatibleBitmap(destino, r.right, r.bottom);
        SelectObject(bufferDC, bufferBitmap);
        bufferL = r.right; bufferA = r.bottom;
    }
    {
        Gdiplus::Graphics g(bufferDC);
        Gdiplus::SolidBrush preto(Gdiplus::Color(255, 0, 0, 0));
        g.FillRectangle(&preto, 0, 0, r.right, r.bottom);
        g.TranslateTransform(Gdiplus::REAL(margemX), Gdiplus::REAL(margemY));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        desenharCena(g);
    }
    BitBlt(destino, 0, 0, r.right, r.bottom, bufferDC, 0, 0, SRCCOPY);
    EndPaint(janela, &ps);
}

LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_CREATE: {
        HMODULE dll = LoadLibraryW(L"xinput1_4.dll");
        if (dll) {
            FARPROC endereco = GetProcAddress(dll, "XInputGetState");
            std::memcpy(&obterEstadoXInput, &endereco, sizeof(obterEstadoXInput));
            bibliotecaXInput = dll;
        }
        SetTimer(janela, 1, 16, nullptr);
        return 0;
    }
    case WM_TIMER:
        relogio = GetTickCount64();
        if (GetForegroundWindow() == janela) processarControleNoMenu();   // só com o menu em foco
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_KEYDOWN:
        if (telaMenu == MENU_PRINCIPAL && (wParam == 'Q' || wParam == VK_ESCAPE)) { DestroyWindow(janela); return 0; }
        ultimoEvento = L"WM_KEYDOWN codigo " + std::to_wstring(wParam) + ((lParam & (1 << 30)) ? L" (repeticao, ignorada)" : L"");
        lerTeclado(wParam, lParam);
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_CHAR:
        ultimoEvento = std::wstring(L"WM_CHAR '") + (wParam >= 32 ? std::wstring(1, wchar_t(wParam)) : L"\\b") + L"'";
        digitarNomeJogador(static_cast<wchar_t>(wParam));
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_LBUTTONUP:
        ultimoEvento = L"WM_LBUTTONUP";
        clicar(static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)));
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) { ajustarAreaDesenho(LOWORD(lParam), HIWORD(lParam)); InvalidateRect(janela, nullptr, FALSE); }
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        desenhar(janela);
        return 0;
    case WM_DESTROY:
        KillTimer(janela, 1);
        if (bufferDC) { DeleteDC(bufferDC); DeleteObject(bufferBitmap); }
        if (bibliotecaXInput) FreeLibrary(bibliotecaXInput);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;
    classe.hInstance = instancia;
    classe.lpszClassName = L"Aula30Menu";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 2; }
    RECT area{0, 0, LARGURA, ALTURA};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 30 - menu completo", WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, area.right - area.left, area.bottom - area.top,
                                  nullptr, nullptr, instancia, nullptr);
    if (!janela) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 3; }
    ShowWindow(janela, exibir);
    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) {
        TranslateMessage(&evento);   // sem esta linha, WM_CHAR nunca chega e o campo de nome não digita
        DispatchMessageW(&evento);
    }
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    return 0;
}
