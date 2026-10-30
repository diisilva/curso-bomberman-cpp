// Aula 33 — Configurando os controles: quem joga com o quê
//
// As telas "QUANTOS VAO JOGAR?" e "QUEM USA CADA CONTROLE?" do jogo real, com as
// mesmas funções de projeto_final/bomberman.cpp:
//   - três estados guardados: doisJogadores, dispositivoP1, dispositivoP2
//   - um estado medido: controleConectado (o jogo não decide; ele observa)
//   - ladoDispositivo/moverDispositivo: cada aparelho fica sob P1, SEM USO ou P2
//   - validarAtribuicao: dois jogadores nunca dividem o mesmo joystick
//   - teclaDoJogador: a escolha vira uma máscara de bits de teclas por jogador
// Sem controle físico, a tecla J liga um JOYSTICK VIRTUAL; K e L são o direcional
// dele. Com um controle Xbox (XInput 0), o direcional, A e B funcionam de verdade.
#include <windows.h>
#include <gdiplus.h>
#include <xinput.h>
#include <algorithm>
#include <cstring>
#include <string>

ULONG_PTR tokenGdiPlus = 0;
const int LARGURA = 760, ALTURA = 520;

// ---------- estado ----------
enum Tela { TELA_QUANTIDADE, TELA_CONTROLES, TELA_RESUMO };
enum Dispositivo { TECLADO, JOYSTICK };
Tela tela = TELA_QUANTIDADE;
bool doisJogadores = false, modoRivais = false;
Dispositivo dispositivoP1 = TECLADO, dispositivoP2 = TECLADO;
bool controleConectado = false;          // medido a cada quadro, nunca escolhido
bool joystickVirtual = false, joystickReal = false;
std::wstring nomeControle;
std::wstring ultimoEvento = L"(nenhum)", avisoTecla;

// ---------- a atribuição (mesmas funções do jogo) ----------
// -1 = P1, 0 = sem uso, 1 = P2.
int ladoDispositivo(Dispositivo d) {
    if (dispositivoP1 == d) return -1;
    if (doisJogadores && dispositivoP2 == d) return 1;
    return 0;
}
// Mover um aparelho para um lado empurra o outro para o lado que sobrou.
void moverDispositivo(Dispositivo d, int sentido) {
    if (!controleConectado) { dispositivoP1 = dispositivoP2 = TECLADO; return; }
    Dispositivo outro = d == TECLADO ? JOYSTICK : TECLADO;
    if (!doisJogadores) dispositivoP1 = sentido < 0 ? d : outro;
    else if (sentido < 0) { dispositivoP1 = d; dispositivoP2 = outro; }
    else { dispositivoP1 = outro; dispositivoP2 = d; }
}
// Opção 0: P1 no teclado (e P2 no joystick). Opção 1: P1 no joystick.
void escolherOpcaoControles(int opcao) { moverDispositivo(TECLADO, opcao == 0 ? -1 : 1); }
void validarAtribuicao() {
    if (!controleConectado) { dispositivoP1 = dispositivoP2 = TECLADO; return; }
    if (doisJogadores && dispositivoP1 == dispositivoP2) dispositivoP2 = dispositivoP1 == TECLADO ? JOYSTICK : TECLADO;
}
void abrirAtribuicaoControles() { validarAtribuicao(); tela = TELA_CONTROLES; }

// ---------- da escolha às teclas ----------
bool controleNoJogador(int numero) {
    if (!controleConectado) return false;   // desconectou: o teclado volta a valer
    if (numero == 1) return dispositivoP1 == JOYSTICK;
    return doisJogadores && dispositivoP2 == JOYSTICK;
}
enum { TECLAS_WASD = 1, TECLAS_SETAS = 2 };
int teclaDoJogador(bool p2) {
    bool p1Teclado = !controleNoJogador(1);
    bool p2Teclado = doisJogadores && !controleNoJogador(2);
    if (!p2) return !p1Teclado ? 0 : p2Teclado ? TECLAS_WASD : TECLAS_WASD | TECLAS_SETAS;
    return !p2Teclado ? 0 : p1Teclado ? TECLAS_SETAS : TECLAS_WASD | TECLAS_SETAS;
}
std::wstring teclasDoJogador(int numero) {
    if (controleNoJogador(numero)) return L"JOYSTICK (A/B bomba)";
    bool outroNoTeclado = doisJogadores && !controleNoJogador(3 - numero);
    if (!outroNoTeclado) return L"WASD/SETAS + ESPACO/ENTER";
    return numero == 1 ? L"WASD + ESPACO" : L"SETAS + ENTER";
}
// Na tela de resumo: de quem é esta tecla? (0 = de ninguém)
int donoDaTecla(WPARAM tecla) {
    int grupo = (tecla == 'W' || tecla == 'A' || tecla == 'S' || tecla == 'D' || tecla == VK_SPACE) ? TECLAS_WASD
              : (tecla == VK_UP || tecla == VK_DOWN || tecla == VK_LEFT || tecla == VK_RIGHT || tecla == VK_RETURN) ? TECLAS_SETAS : 0;
    if (!grupo) return -1;                       // não é tecla de jogo
    if (teclaDoJogador(false) & grupo) return 1;
    if (doisJogadores && (teclaDoJogador(true) & grupo)) return 2;
    return 0;
}

// ---------- conexão: o que muda quando o controle entra ou sai ----------
void atualizarConexao(bool real) {
    bool antes = controleConectado;
    joystickReal = real;
    controleConectado = joystickReal || joystickVirtual;
    nomeControle = joystickReal ? L"Controle XInput 0" : joystickVirtual ? L"Joystick virtual (J)" : L"";
    // O jogo só valida ao abrir a tela e no Enter. Validar também aqui evita
    // mostrar o joystick em SEM USO com 2 jogadores (ver "O que mudou").
    if (controleConectado != antes && tela == TELA_CONTROLES) validarAtribuicao();
}

// ---------- teclado: o único lugar que muda a tela ----------
void lerTeclado(WPARAM tecla, LPARAM lParam) {
    if (lParam & (1 << 30)) return;              // repetição automática (Aula 30)
    if (tecla == 'J') {                          // liga/desliga o joystick virtual em qualquer tela
        joystickVirtual = !joystickVirtual;
        atualizarConexao(joystickReal);
        return;
    }
    if (tela == TELA_QUANTIDADE) {
        int escolha = tecla >= '1' && tecla <= '3' ? int(tecla - '1') : -1;
        int atual = !doisJogadores ? 0 : modoRivais ? 2 : 1;
        if (tecla == VK_LEFT || tecla == VK_UP) atual = (atual + 2) % 3;
        else if (tecla == VK_RIGHT || tecla == VK_DOWN) atual = (atual + 1) % 3;
        if (escolha >= 0) atual = escolha;
        doisJogadores = atual > 0;
        modoRivais = atual == 2;
        // No jogo, a tela de nomes vem aqui no meio (Aula 30).
        if (escolha >= 0 || tecla == VK_RETURN) abrirAtribuicaoControles();
        return;
    }
    if (tela == TELA_CONTROLES) {
        if (tecla == VK_LEFT || tecla == 'A') moverDispositivo(TECLADO, -1);
        else if (tecla == VK_RIGHT || tecla == 'D') moverDispositivo(TECLADO, 1);
        else if (tecla == 'K') { if (controleConectado) moverDispositivo(JOYSTICK, -1); }   // direcional do joystick virtual
        else if (tecla == 'L') { if (controleConectado) moverDispositivo(JOYSTICK, 1); }
        else if (tecla == '1') escolherOpcaoControles(0);
        else if (tecla == '2') escolherOpcaoControles(1);
        else if (tecla == VK_RETURN) { validarAtribuicao(); tela = TELA_RESUMO; avisoTecla.clear(); }
        else if (tecla == 'M' || tecla == VK_ESCAPE) tela = TELA_QUANTIDADE;
        return;
    }
    // TELA_RESUMO: aperte as teclas de jogo para ver quem as recebe.
    int dono = donoDaTecla(tecla);
    if (dono >= 0) {
        avisoTecla = dono == 0 ? L"essa tecla nao move ninguem: o dono dela esta no joystick"
                               : L"essa tecla e do P" + std::to_wstring(dono);
        return;
    }
    if (tecla == 'M' || tecla == VK_ESCAPE) abrirAtribuicaoControles();
}

// ---------- joystick real: só o toque conta (Aula 31) ----------
using FuncaoXInputGetState = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
HMODULE bibliotecaXInput = nullptr;
FuncaoXInputGetState obterEstadoXInput = nullptr;
WORD botoesAnteriores = 0;
bool novo(bool agora, bool antes) { return agora && !antes; }

void processarControleNoMenu(bool janelaEmFoco) {
    XINPUT_STATE estado{};
    bool real = obterEstadoXInput && obterEstadoXInput(0, &estado) == ERROR_SUCCESS;
    bool acabouDeConectar = real && !joystickReal;
    atualizarConexao(real);
    WORD b = real && janelaEmFoco ? estado.Gamepad.wButtons : 0;
    if (acabouDeConectar) botoesAnteriores = b;  // botão já apertado ao conectar não é toque
    WORD antes = botoesAnteriores;
    botoesAnteriores = b;
    auto toque = [&](WORD botao) { return novo(b & botao, antes & botao); };
    int dc = toque(XINPUT_GAMEPAD_DPAD_LEFT) ? -1 : toque(XINPUT_GAMEPAD_DPAD_RIGHT) ? 1 : 0;
    int dl = toque(XINPUT_GAMEPAD_DPAD_UP) ? -1 : toque(XINPUT_GAMEPAD_DPAD_DOWN) ? 1 : 0;
    // Como no jogo: na tela de controles o direcional move o cartão do PRÓPRIO joystick.
    if (dc && tela == TELA_CONTROLES) { ultimoEvento = L"joystick: direcional move o JOYSTICK"; moverDispositivo(JOYSTICK, dc); }
    else if (dc || dl) { ultimoEvento = L"joystick: direcional"; lerTeclado(dl < 0 ? VK_UP : dl > 0 ? VK_DOWN : dc < 0 ? VK_LEFT : VK_RIGHT, 0); }
    else if (toque(XINPUT_GAMEPAD_A)) { ultimoEvento = L"joystick: A"; lerTeclado(VK_RETURN, 0); }
    else if (toque(XINPUT_GAMEPAD_B)) { ultimoEvento = L"joystick: B"; lerTeclado(VK_ESCAPE, 0); }
}

// ---------- escala e mouse ----------
float escala = 1.0f;
int margemX = 0, margemY = 0;
int px(int v) { return static_cast<int>(v * escala + 0.5f); }
void ajustarAreaDesenho(int largura, int altura) {
    if (largura <= 0 || altura <= 0) return;
    escala = std::min(float(largura) / LARGURA, float(altura) / ALTURA);
    margemX = (largura - px(LARGURA)) / 2;
    margemY = (altura - px(ALTURA)) / 2;
}
void clicar(int xJanela, int yJanela) {
    int x = static_cast<int>((xJanela - margemX) / escala);
    int y = static_cast<int>((yJanela - margemY) / escala);
    if (tela == TELA_QUANTIDADE) {
        if (y >= 110 && y < 360 && x >= 30 && x < 730 && (x - 30) % 240 < 220) lerTeclado('1' + (x - 30) / 240, 0);
    } else if (tela == TELA_CONTROLES) {
        if (y >= 330 && y < 370 && x >= 30 && x < 730 && (x - 30) % 355 < 345) lerTeclado('1' + (x - 30) / 355, 0);
        else if (y >= 390 && y < 430 && x >= 30 && x < 730) lerTeclado(VK_RETURN, 0);
        else if (y >= 90 && y < 315 && x >= 30 && x < 730 && controleConectado) {
            int coluna = (x - 30) / 240;                 // clicar numa coluna leva o TECLADO para lá
            if (coluna == 0) moverDispositivo(TECLADO, -1);
            else if (coluna == 2 || (coluna == 1 && !doisJogadores)) moverDispositivo(TECLADO, 1);
        }
    }
}

// ---------- desenho: só lê o estado ----------
const Gdiplus::Color VERDE(255, 98, 235, 199), DOURADO(255, 252, 187, 65), CINZA(255, 144, 164, 190),
    CLARO(255, 220, 230, 240), APAGADO(255, 70, 80, 95), VERMELHO(255, 255, 120, 100), AZUL(255, 110, 157, 186);
void painel(Gdiplus::Graphics& g, int x, int y, int l, int a, Gdiplus::Color cor = Gdiplus::Color(255, 25, 34, 50)) {
    Gdiplus::SolidBrush pincel(cor);
    g.FillRectangle(&pincel, px(x), px(y), px(l), px(a));
}
void desenharTexto(Gdiplus::Graphics& g, const std::wstring& texto, int x, int y, int tamanho, Gdiplus::Color cor = CLARO) {
    Gdiplus::Font fonte(L"Segoe UI", Gdiplus::REAL(px(tamanho)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(texto.c_str(), -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(x)), Gdiplus::REAL(px(y))), &pincel);
}
std::wstring situacaoControle() {
    return controleConectado ? L"Joystick conectado: " + nomeControle : L"Nenhum joystick conectado. Aperte J para ligar o virtual.";
}

void desenharQuantidade(Gdiplus::Graphics& g) {
    desenharTexto(g, L"QUANTOS VAO JOGAR?", 30, 26, 28, Gdiplus::Color(255, 246, 247, 251));
    desenharTexto(g, L"Plugar um controle nunca cria jogador: so esta escolha cria.", 30, 70, 14, AZUL);
    const wchar_t* titulos[] = {L"1   1 JOGADOR", L"2   AMIGOS", L"3   INIMIGOS"};
    const wchar_t* resumos[] = {L"Teclado OU joystick,\nvoce escolhe.", L"2 jogadores no\nmesmo time.", L"Todos contra todos,\npontos separados."};
    int atual = !doisJogadores ? 0 : modoRivais ? 2 : 1;
    for (int i = 0; i < 3; ++i) {
        int x = 30 + i * 240;
        bool marcado = atual == i;
        Gdiplus::Color cor = marcado ? (i == 2 ? VERMELHO : VERDE) : Gdiplus::Color(255, 55, 68, 86);
        painel(g, x, 110, 220, 250);
        painel(g, x, 110, 220, 4, cor);
        desenharTexto(g, titulos[i], x + 16, 128, 20, marcado ? cor : CLARO);
        Gdiplus::Font fonte(L"Segoe UI", Gdiplus::REAL(px(14)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush pincel(CLARO);
        g.DrawString(resumos[i], -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(x + 16)), Gdiplus::REAL(px(180))), &pincel);
        desenharTexto(g, marcado ? L"SELECIONADO" : L"ESCOLHER", x + 16, 320, 14, cor);
    }
    painel(g, 30, 380, 700, 40, Gdiplus::Color(255, 23, 48, 62));
    desenharTexto(g, doisJogadores && !controleConectado ? L"Sem joystick: os dois dividem o teclado (WASD + ESPACO / SETAS + ENTER)." : situacaoControle(),
                  44, 390, 14, DOURADO);
    desenharTexto(g, L"1 / 2 / 3, SETAS + ENTER ou CLIQUE     J joystick virtual     ESC sai", 30, 440, 13, CINZA);
}

void desenharControles(Gdiplus::Graphics& g) {
    desenharTexto(g, L"QUEM USA CADA CONTROLE?", 30, 20, 28, Gdiplus::Color(255, 246, 247, 251));
    desenharTexto(g, doisJogadores ? L"2 JOGADORES: leve o teclado e o joystick para o lado de cada jogador."
                                   : L"1 JOGADOR: leve para P1 o controle que voce vai usar.", 30, 62, 14, AZUL);
    const wchar_t* titulos[] = {L"P1", L"SEM USO", L"P2"};
    Gdiplus::Color coresLado[] = {VERDE, Gdiplus::Color(255, 110, 120, 135), DOURADO};
    for (int i = 0; i < 3; ++i) {
        int x = 30 + i * 240;
        bool inativa = i == 2 && !doisJogadores;
        painel(g, x, 90, 220, 225, inativa ? Gdiplus::Color(255, 18, 23, 33) : Gdiplus::Color(255, 25, 34, 50));
        painel(g, x, 90, 220, 4, inativa ? Gdiplus::Color(255, 45, 52, 64) : coresLado[i]);
        desenharTexto(g, titulos[i], x + 14, 100, 18, inativa ? APAGADO : coresLado[i]);
        if (inativa) desenharTexto(g, L"so com 2 jogadores", x + 14, 126, 11, APAGADO);
    }
    auto cartao = [&](int lado, int y, const std::wstring& nome, const std::wstring& detalhe, bool disponivel) {
        int x = 30 + (lado + 1) * 240 + 12;
        Gdiplus::Color cor = !disponivel ? APAGADO : lado == 0 ? CINZA : coresLado[lado + 1];
        painel(g, x, y, 196, 76, disponivel ? Gdiplus::Color(255, 35, 48, 68) : Gdiplus::Color(255, 24, 30, 42));
        painel(g, x, y, 4, 76, cor);
        desenharTexto(g, nome, x + 14, y + 8, 17, cor);
        desenharTexto(g, detalhe, x + 14, y + 40, 11);
    };
    if (doisJogadores && !controleConectado) {
        cartao(-1, 140, L"TECLADO", L"WASD + ESPACO", true);
        cartao(1, 140, L"TECLADO", L"SETAS + ENTER", true);
    } else {
        int lado = ladoDispositivo(TECLADO);
        cartao(lado, 140, L"TECLADO", lado == 0 ? L"nao sera usado" : teclasDoJogador(lado < 0 ? 1 : 2), true);
    }
    if (controleConectado) {
        int lado = ladoDispositivo(JOYSTICK);
        cartao(lado, 228, L"JOYSTICK", lado == 0 ? L"nao sera usado" : nomeControle, true);
    } else cartao(0, 228, L"JOYSTICK", L"nao detectado", false);
    for (int i = 0; i < 2; ++i) {
        int x = 30 + i * 355;
        bool habilitada = controleConectado || i == 0;
        bool marcada = habilitada && (i == 0) == (dispositivoP1 == TECLADO || !controleConectado);
        Gdiplus::Color cor = !habilitada ? APAGADO : marcada ? VERDE : CLARO;
        painel(g, x, 330, 345, 40, marcada ? Gdiplus::Color(255, 30, 62, 66) : Gdiplus::Color(255, 25, 34, 50));
        painel(g, x, 330, 4, 40, cor);
        std::wstring texto = doisJogadores ? (i == 0 ? (controleConectado ? L"1   P1 TECLADO + P2 JOYSTICK" : L"1   P1 E P2 NO TECLADO") : L"2   P1 JOYSTICK + P2 TECLADO")
                                           : (i == 0 ? L"1   P1 NO TECLADO" : L"2   P1 NO JOYSTICK");
        desenharTexto(g, texto, x + 16, 339, 14, cor);
    }
    painel(g, 30, 390, 700, 40, Gdiplus::Color(255, 30, 62, 66));
    painel(g, 30, 390, 700, 3, VERDE);
    desenharTexto(g, L"ENTER   CONFIRMAR", 46, 399, 16, VERDE);
    desenharTexto(g, situacaoControle(), 250, 401, 12, DOURADO);
    desenharTexto(g, L"SETAS ou A/D movem o TECLADO    K/L (ou direcional) movem o JOYSTICK    1/2 atalhos    J liga/desliga    ESC volta",
                  30, 440, 11, CINZA);
}

std::wstring mascaraEmTexto(int m) {
    std::wstring bits = std::wstring(1, (m & TECLAS_SETAS) ? L'1' : L'0') + ((m & TECLAS_WASD) ? L'1' : L'0');
    std::wstring nomes = m == 0 ? L"nenhuma" : m == TECLAS_WASD ? L"WASD" : m == TECLAS_SETAS ? L"SETAS" : L"WASD | SETAS";
    return L"teclaDoJogador = " + std::to_wstring(m) + L"  (bits " + bits + L" = " + nomes + L")";
}
void desenharResumo(Gdiplus::Graphics& g) {
    desenharTexto(g, L"RESUMO: QUEM JOGA COM O QUE", 30, 20, 26, Gdiplus::Color(255, 246, 247, 251));
    desenharTexto(g, std::wstring(L"guardado:  dispositivoP1 = ") + (dispositivoP1 == TECLADO ? L"TECLADO" : L"JOYSTICK") +
                     (doisJogadores ? std::wstring(L"   dispositivoP2 = ") + (dispositivoP2 == TECLADO ? L"TECLADO" : L"JOYSTICK") : L"") +
                     L"   conectado = " + (controleConectado ? L"sim" : L"nao"), 30, 62, 13, AZUL);
    for (int n = 1; n <= (doisJogadores ? 2 : 1); ++n) {
        int y = 96 + (n - 1) * 110;
        Gdiplus::Color cor = n == 1 ? VERDE : DOURADO;
        painel(g, 30, y, 700, 96);
        painel(g, 30, y, 4, 96, cor);
        desenharTexto(g, L"P" + std::to_wstring(n), 48, y + 10, 22, cor);
        desenharTexto(g, teclasDoJogador(n), 110, y + 14, 18);
        desenharTexto(g, controleNoJogador(n) ? std::wstring(L"controleNoJogador(") + std::to_wstring(n) + L") = true: anda pelo joystick"
                                              : mascaraEmTexto(teclaDoJogador(n == 2)), 110, y + 52, 13, CINZA);
    }
    painel(g, 30, 330, 700, 50, Gdiplus::Color(255, 23, 48, 62));
    desenharTexto(g, avisoTecla.empty() ? L"Aperte W, A, S, D, ESPACO, SETAS ou ENTER para ver de quem e cada tecla." : avisoTecla, 44, 344, 15, DOURADO);
    desenharTexto(g, L"Aperte J aqui: desconectar NAO apaga a escolha guardada, so devolve o teclado ao dono.", 30, 396, 13, CLARO);
    desenharTexto(g, L"No jogo, o proximo passo e escolher personagens.      M ou ESC volta aos controles", 30, 440, 13, CINZA);
}

void desenharCena(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 11, 16, 28));
    painel(g, 0, 0, LARGURA, 5, DOURADO);
    if (tela == TELA_QUANTIDADE) desenharQuantidade(g);
    else if (tela == TELA_CONTROLES) desenharControles(g);
    else desenharResumo(g);
    painel(g, 0, 476, LARGURA, 44, Gdiplus::Color(255, 18, 27, 43));
    desenharTexto(g, L"ultimo evento: " + ultimoEvento, 30, 490, 13, DOURADO);
    desenharTexto(g, obterEstadoXInput ? L"XInput pronto (controle 0)" : L"sem XInput", 560, 490, 13, CINZA);
}

// ---------- janela ----------
HDC bufferDC = nullptr; HBITMAP bufferBitmap = nullptr; int bufferL = 0, bufferA = 0;
void desenhar(HWND janela) {
    PAINTSTRUCT ps;
    HDC destino = BeginPaint(janela, &ps);
    RECT r; GetClientRect(janela, &r);
    if (!bufferDC || bufferL != r.right || bufferA != r.bottom) {
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
        processarControleNoMenu(GetForegroundWindow() == janela);
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_KEYDOWN:
        if (tela == TELA_QUANTIDADE && wParam == VK_ESCAPE) { DestroyWindow(janela); return 0; }
        ultimoEvento = L"WM_KEYDOWN codigo " + std::to_wstring(wParam);
        lerTeclado(wParam, lParam);
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
    classe.lpszClassName = L"Aula33Controles";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 2; }
    RECT area{0, 0, LARGURA, ALTURA};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 33 - configuracao de controles", WS_OVERLAPPEDWINDOW,
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
