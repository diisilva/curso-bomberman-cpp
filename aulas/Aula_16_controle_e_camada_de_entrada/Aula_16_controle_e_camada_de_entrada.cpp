// Aula 16 — Controle e camada de entrada
//
// O jogo não deixa as regras lerem o controle diretamente. Tudo vira uma
// EntradaControle ("ande para cima", "bomba", "voltar"), venha de onde vier.
// direcaoDoControle, montarEntrada e traduzirXInput são copiadas do jogo
// real (projeto_final/bomberman.cpp). Os testes simulam o controle, então o
// programa funciona sem nenhum aparelho; no fim, ele lê o controle 0 de verdade
// se houver um XInput conectado.
#include <windows.h>
#include <xinput.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct EntradaControle { int dl = 0, dc = 0; bool bomba = false, confirmar = false, voltar = false, chute = false,
                         teleporte = false, detonar = false, start = false, select = false; };

// Analógico → uma das 4 direções. Abaixo do limite em X e Y: parado (zona morta).
// Fora dela, vence o eixo mais inclinado: nada de diagonais num jogo de grade.
EntradaControle direcaoDoControle(int x, int y, int limite) {
    EntradaControle e;
    if (abs(x) < limite && abs(y) < limite) return e;
    if (abs(x) > abs(y)) e.dc = x > 0 ? 1 : -1;
    else e.dl = y > 0 ? 1 : -1;
    return e;
}

// O formato comum: direcional tem prioridade; senão, o analógico.
// Botões no layout padrão: A/B bomba, X chute, Y teleporte, ombros detonar.
EntradaControle montarEntrada(bool cima, bool baixo, bool esq, bool dir, int x, int y, int limite,
                              bool a, bool b, bool xb, bool yb, bool ombro, bool start, bool select) {
    EntradaControle e;
    if (cima) e.dl = -1; else if (baixo) e.dl = 1; else if (esq) e.dc = -1; else if (dir) e.dc = 1;
    else e = direcaoDoControle(x, y, limite);
    e.confirmar = a; e.voltar = b; e.bomba = a || b; e.chute = xb; e.teleporte = yb; e.detonar = ombro;
    e.start = start; e.select = select;
    return e;
}

// XInput: Y do analógico cresce para CIMA, a linha da matriz cresce para BAIXO,
// por isso o sinal trocado em -int(g.sThumbLY). Zona morta: 10500 de 32767.
EntradaControle traduzirXInput(const XINPUT_GAMEPAD& g) {
    WORD b = g.wButtons;
    return montarEntrada(b & XINPUT_GAMEPAD_DPAD_UP, b & XINPUT_GAMEPAD_DPAD_DOWN, b & XINPUT_GAMEPAD_DPAD_LEFT,
                         b & XINPUT_GAMEPAD_DPAD_RIGHT, int(g.sThumbLX), -int(g.sThumbLY), 10500,
                         b & XINPUT_GAMEPAD_A, b & XINPUT_GAMEPAD_B, b & XINPUT_GAMEPAD_X, b & XINPUT_GAMEPAD_Y,
                         b & (XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER),
                         b & XINPUT_GAMEPAD_START, b & XINPUT_GAMEPAD_BACK);
}

void mostrar(const char* caso, const EntradaControle& e) {
    std::printf("%-38s -> dl=%2d dc=%2d bomba=%d chute=%d voltar=%d\n", caso, e.dl, e.dc, e.bomba, e.chute, e.voltar);
}

XINPUT_GAMEPAD controle(WORD botoes, SHORT x, SHORT y) {
    XINPUT_GAMEPAD g{};
    g.wButtons = botoes; g.sThumbLX = x; g.sThumbLY = y;
    return g;
}

int main() {
    mostrar("direcional para cima + A", traduzirXInput(controle(XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_A, 0, 0)));
    mostrar("analogico (22000, 2000) + B", traduzirXInput(controle(XINPUT_GAMEPAD_B, 22000, 2000)));
    mostrar("analogico (5000, 0) + X: zona morta", traduzirXInput(controle(XINPUT_GAMEPAD_X, 5000, 0)));
    mostrar("analogico diagonal (15000, 20000)", traduzirXInput(controle(0, 15000, 20000)));
    mostrar("direcional esquerda + analogico direita", traduzirXInput(controle(XINPUT_GAMEPAD_DPAD_LEFT, 30000, 0)));

    // Leitura real, se houver controle. A DLL é carregada em tempo de execução
    // (Aula 31 explica por quê); sem ela, o programa continua funcionando.
    HMODULE dll = LoadLibraryW(L"xinput1_4.dll");
    if (!dll) { std::printf("XInput indisponivel neste Windows\n"); return 0; }
    using FuncaoXInputGetState = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
    FARPROC endereco = GetProcAddress(dll, "XInputGetState");
    FuncaoXInputGetState obterEstado = nullptr;
    std::memcpy(&obterEstado, &endereco, sizeof(obterEstado));   // ver Aula 29: ponteiro de função
    XINPUT_STATE estado{};
    if (obterEstado && obterEstado(0, &estado) == ERROR_SUCCESS) mostrar("controle 0, agora", traduzirXInput(estado.Gamepad));
    else std::printf("nenhum controle XInput conectado (os testes acima nao precisam de um)\n");
    FreeLibrary(dll);
    return 0;
}
