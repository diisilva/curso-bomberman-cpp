// Aula 16 — solução do exercício 2: teclado e controle no mesmo formato.
//
// traduzirTeclado transforma o vetor teclaPressionada (Aula 03) numa
// EntradaControle. A partir daí, a regra (aplicar) não sabe — e não precisa
// saber — de onde veio o comando. No jogo real, os dois caminhos se juntam um
// passo depois: processarMovimentoContinuo chama moverJogador tanto para as
// teclas quanto para entradaControle.
#include <windows.h>
#include <xinput.h>
#include <cstdio>
#include <cstdlib>

struct EntradaControle { int dl = 0, dc = 0; bool bomba = false; };

EntradaControle direcaoDoControle(int x, int y, int limite) {
    EntradaControle e;
    if (abs(x) < limite && abs(y) < limite) return e;
    if (abs(x) > abs(y)) e.dc = x > 0 ? 1 : -1; else e.dl = y > 0 ? 1 : -1;
    return e;
}
EntradaControle traduzirXInput(const XINPUT_GAMEPAD& g) {
    EntradaControle e = direcaoDoControle(g.sThumbLX, -int(g.sThumbLY), 10500);
    e.bomba = (g.wButtons & (XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B)) != 0;
    return e;
}

bool teclaPressionada[256] = {};
EntradaControle traduzirTeclado() {
    EntradaControle e;
    if (teclaPressionada['W']) e.dl = -1; else if (teclaPressionada['S']) e.dl = 1;
    else if (teclaPressionada['A']) e.dc = -1; else if (teclaPressionada['D']) e.dc = 1;
    e.bomba = teclaPressionada[VK_SPACE];
    return e;
}

// A "regra": só conhece EntradaControle.
struct Posicao { int linha, coluna; };
void aplicar(Posicao& jogador, int& bombas, const EntradaControle& e) {
    jogador.linha += e.dl;
    jogador.coluna += e.dc;
    if (e.bomba) bombas++;
}

int main() {
    Posicao pelo_teclado{5, 5}, pelo_controle{5, 5};
    int bombasTeclado = 0, bombasControle = 0;

    teclaPressionada['D'] = true; teclaPressionada[VK_SPACE] = true;
    XINPUT_GAMEPAD g{}; g.sThumbLX = 25000; g.wButtons = XINPUT_GAMEPAD_A;

    aplicar(pelo_teclado, bombasTeclado, traduzirTeclado());
    aplicar(pelo_controle, bombasControle, traduzirXInput(g));
    std::printf("teclado:  jogador=(%d,%d) bombas=%d\n", pelo_teclado.linha, pelo_teclado.coluna, bombasTeclado);
    std::printf("controle: jogador=(%d,%d) bombas=%d\n", pelo_controle.linha, pelo_controle.coluna, bombasControle);
    return 0;
}
