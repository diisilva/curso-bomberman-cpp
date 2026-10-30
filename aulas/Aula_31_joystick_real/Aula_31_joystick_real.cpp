// Aula 31 — Conectando um joystick de verdade
//
// O sistema de controle do jogo real (projeto_final/bomberman.cpp, ~240 linhas)
// em sete peças, todas testáveis SEM controle conectado:
//   1. qual API atende: SDL2, senão XInput, senão DirectInput
//   2. DirectInput: eixos com faixas diferentes e direcional em graus
//   3. Switch Pro: reconhecido pelo VID/PID e recusado no DirectInput
//   4. toque: a ação dispara quando o botão DESCE, não enquanto está apertado
//   5. reconexão: um botão já apertado ao conectar não conta como toque
//   6. foco: com outra janela na frente, o controle não age
//   7. quem usa o quê: teclado e controle divididos entre P1 e P2
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

struct EntradaControle { int dl = 0, dc = 0; bool bomba = false, confirmar = false, voltar = false, chute = false,
                         teleporte = false, detonar = false, start = false, select = false; };

EntradaControle direcaoDoControle(int x, int y, int limite) {
    EntradaControle e;
    if (abs(x) < limite && abs(y) < limite) return e;
    if (abs(x) > abs(y)) e.dc = x > 0 ? 1 : -1; else e.dl = y > 0 ? 1 : -1;
    return e;
}
EntradaControle montarEntrada(bool cima, bool baixo, bool esq, bool dir, int x, int y, int limite,
                              bool a, bool b, bool xb, bool yb, bool ombro, bool start, bool select) {
    EntradaControle e;
    if (cima) e.dl = -1; else if (baixo) e.dl = 1; else if (esq) e.dc = -1; else if (dir) e.dc = 1;
    else e = direcaoDoControle(x, y, limite);
    e.confirmar = a; e.voltar = b; e.bomba = a || b; e.chute = xb; e.teleporte = yb; e.detonar = ombro;
    e.start = start; e.select = select;
    return e;
}

// ---------- 1. qual API atende ----------
enum OrigemControle { SEM_CONTROLE, CONTROLE_SDL, CONTROLE_XINPUT, CONTROLE_GENERICO };
const char* nomeOrigem[] = {"nenhum", "SDL2", "XInput", "DirectInput"};
// O mesmo encadeamento de atualizarEstadoControle: a primeira que achar controle vence.
OrigemControle escolherOrigem(bool sdlTemControle, bool xinputTemControle, bool dinputTemControle) {
    if (sdlTemControle) return CONTROLE_SDL;
    if (xinputTemControle) return CONTROLE_XINPUT;
    if (dinputTemControle) return CONTROLE_GENERICO;
    return SEM_CONTROLE;
}

// ---------- 2. DirectInput ----------
struct FaixaEixo { LONG minimo = -1000, maximo = 1000; bool disponivel = true; };
// Cada controle genérico informa uma faixa própria (0..65535, -32768..32767...).
// O jogo leva tudo para -1000..1000 antes de traduzir.
LONG normalizarEixoControle(LONG valor, const FaixaEixo& faixa) {
    if (!faixa.disponivel || faixa.maximo <= faixa.minimo) return 0;
    long long escala = (static_cast<long long>(valor) - faixa.minimo) * 2000 / (faixa.maximo - faixa.minimo) - 1000;
    return static_cast<LONG>(std::clamp(escala, -1000LL, 1000LL));
}
// O direcional (POV) vem em centésimos de grau: 0 = cima, 9000 = direita... 0xFFFFFFFF = solto.
EntradaControle traduzirDirectInput(const DIJOYSTATE2& g) {
    DWORD pov = g.rgdwPOV[0];
    bool cima = false, baixo = false, esq = false, dir = false;
    if (pov != 0xffffffff) {
        if (pov <= 4500 || pov >= 31500) cima = true;
        else if (pov >= 13500 && pov <= 22500) baixo = true;
        else if (pov < 13500) dir = true;
        else esq = true;
    }
    auto botao = [&](int i) { return (g.rgbButtons[i] & 0x80) != 0; };
    EntradaControle e = montarEntrada(cima, baixo, esq, dir, int(g.lX), int(g.lY), 350, botao(0), botao(1), botao(2), botao(3),
                                      botao(4) || botao(5), botao(9), botao(8));
    if (!e.dl && !e.dc) {   // sem direcional nem analógico esquerdo: tenta o direito
        EntradaControle direita = direcaoDoControle(int(g.lRx), int(g.lRy), 350);
        e.dl = direita.dl; e.dc = direita.dc;
    }
    return e;
}

// ---------- 3. Switch Pro ----------
// guidProduct.Data1 junta o fabricante (VID, 16 bits baixos) e o produto (PID, 16 altos).
bool ehSwitchPro(DWORD data1) { return LOWORD(data1) == 0x057e && HIWORD(data1) == 0x2009; }

// ---------- 4 e 5. toque e reconexão ----------
// processarControleNoMenu: auto novo = [](bool agora, bool antes) { return agora && !antes; };
bool novo(bool agora, bool antes) { return agora && !antes; }

// ---------- 7. quem usa o quê ----------
enum Dispositivo { TECLADO, JOYSTICK };
enum { TECLAS_WASD = 1, TECLAS_SETAS = 2 };
bool controleConectado = true, doisJogadores = false;
Dispositivo dispositivoP1 = TECLADO, dispositivoP2 = TECLADO;
bool controleNoJogador(int numero) {
    if (!controleConectado) return false;
    if (numero == 1) return dispositivoP1 == JOYSTICK;
    return doisJogadores && dispositivoP2 == JOYSTICK;
}
int teclaDoJogador(bool p2) {
    bool p1Teclado = !controleNoJogador(1);
    bool p2Teclado = doisJogadores && !controleNoJogador(2);
    if (!p2) return !p1Teclado ? 0 : p2Teclado ? TECLAS_WASD : TECLAS_WASD | TECLAS_SETAS;
    return !p2Teclado ? 0 : p1Teclado ? TECLAS_SETAS : TECLAS_WASD | TECLAS_SETAS;
}
const char* teclas(int mascara) {
    switch (mascara) {
        case 0: return "joystick";
        case TECLAS_WASD: return "WASD";
        case TECLAS_SETAS: return "setas";
        default: return "WASD e setas";
    }
}

int main() {
    std::printf("1. qual API atende (SDL2 > XInput > DirectInput)\n");
    std::printf("   controle Xbox, SDL2.dll presente   -> %s\n", nomeOrigem[escolherOrigem(true, true, true)]);
    std::printf("   controle Xbox, sem SDL2.dll         -> %s\n", nomeOrigem[escolherOrigem(false, true, true)]);
    std::printf("   controle USB generico, sem SDL2.dll -> %s\n", nomeOrigem[escolherOrigem(false, false, true)]);

    std::printf("2. DirectInput\n");
    FaixaEixo faixa{0, 65535, true};
    std::printf("   eixo 0..65535: 0 -> %ld, 32767 -> %ld, 65535 -> %ld\n",
                normalizarEixoControle(0, faixa), normalizarEixoControle(32767, faixa), normalizarEixoControle(65535, faixa));
    DIJOYSTATE2 g{};
    for (auto& p : g.rgdwPOV) p = 0xffffffff;
    for (DWORD graus : {0u, 9000u, 18000u, 27000u, 4500u}) {
        g.rgdwPOV[0] = graus;
        EntradaControle e = traduzirDirectInput(g);
        std::printf("   POV %5lu (%3lu graus) -> dl=%2d dc=%2d\n", graus, graus / 100, e.dl, e.dc);
    }

    std::printf("3. Switch Pro (VID 057e, PID 2009)\n");
    std::printf("   Data1=0x2009057e -> switch=%d; Data1=0x02e1045e (Xbox) -> switch=%d\n",
                ehSwitchPro(0x2009057e), ehSwitchPro(0x02e1045e));

    std::printf("4. toque: a bomba sai so quando o botao desce\n   quadros:");
    const bool botaoA[] = {false, true, true, true, false, true};
    bool antes = false;
    int bombas = 0;
    for (bool agora : botaoA) {
        bool toque = novo(agora, antes);
        bombas += toque;
        std::printf(" %s", agora ? (toque ? "[A!]" : "[A]") : "[ ]");
        antes = agora;
    }
    std::printf("  -> %d bombas em 6 quadros\n", bombas);

    std::printf("5. reconexao com o botao ja apertado\n");
    bool estavaConectado = false, conectado = true, leituraA = true, anteriorA = false;
    if (!conectado || !estavaConectado) anteriorA = leituraA;   // o jogo copia a leitura atual para "antes"
    std::printf("   conectou segurando A: toque=%d (sem bomba fantasma)\n", novo(leituraA, anteriorA));

    std::printf("6. foco\n");
    EntradaControle leitura = montarEntrada(true, false, false, false, 0, 0, 1, true, false, false, false, false, false, false);
    bool janelaNaFrente = false;
    if (!janelaNaFrente) leitura = EntradaControle{};   // if (GetForegroundWindow() != janelaPrincipal)
    std::printf("   outra janela na frente: dl=%d bomba=%d\n", leitura.dl, leitura.bomba);

    std::printf("7. quem usa o que\n");
    struct Caso { const char* nome; bool dois, conectado; Dispositivo p1, p2; };
    const Caso casos[] = {
        {"1 jogador, teclado          ", false, true, TECLADO, TECLADO},
        {"1 jogador, joystick         ", false, true, JOYSTICK, TECLADO},
        {"2 jogadores, sem joystick   ", true, false, TECLADO, TECLADO},
        {"2 jogadores, P2 no joystick ", true, true, TECLADO, JOYSTICK},
        {"2 jogadores, P1 no joystick ", true, true, JOYSTICK, TECLADO},
    };
    for (const Caso& c : casos) {
        doisJogadores = c.dois; controleConectado = c.conectado; dispositivoP1 = c.p1; dispositivoP2 = c.p2;
        std::printf("   %s P1: %-12s", c.nome, teclas(teclaDoJogador(false)));
        if (c.dois) std::printf(" P2: %s", teclas(teclaDoJogador(true)));
        std::printf("\n");
    }
    return 0;
}
