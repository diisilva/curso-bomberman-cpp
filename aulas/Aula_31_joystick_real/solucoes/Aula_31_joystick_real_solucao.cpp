// Aula 31 — solução do exercício 3: repetir a navegação ao segurar.
//
// Em processarControleNoMenu, a direção só gera um evento quando MUDA
// (direcaoNova). Segurar o direcional para baixo numa lista longa (o ranking,
// por exemplo) exige apertar várias vezes. Os teclados resolvem isso com a
// repetição automática: espera ~400 ms e depois repete a cada ~120 ms.
#include <cstdio>

struct RepeticaoDirecao {
    int direcaoAnterior = 0;   // 0 = solto; 1..4 = cima, baixo, esquerda, direita
    int tempoMs = 0;
    // Devolve true nos quadros em que o menu deve andar um item.
    bool passo(int direcao, int dtMs) {
        if (direcao == 0) { direcaoAnterior = 0; return false; }
        if (direcao != direcaoAnterior) { direcaoAnterior = direcao; tempoMs = -400; return true; }   // 1º evento
        tempoMs += dtMs;
        if (tempoMs >= 120) { tempoMs -= 120; return true; }   // repetições
        return false;
    }
};

int main() {
    // O jogador segura "baixo" por 1 segundo (quadros de 16 ms) e solta.
    int eventosJogo = 0, eventosRepeticao = 0, direcaoAnteriorJogo = 0;
    RepeticaoDirecao repeticao;
    std::printf("eventos com repeticao em t =");
    for (int t = 0; t <= 1000; t += 16) {
        int direcao = 2;
        if (direcao != direcaoAnteriorJogo) eventosJogo++;   // regra do jogo: só quando muda
        direcaoAnteriorJogo = direcao;
        if (repeticao.passo(direcao, 16)) { eventosRepeticao++; std::printf(" %d", t); }
    }
    std::printf(" ms\nsegurando 1 s: jogo=%d evento, com repeticao=%d eventos\n", eventosJogo, eventosRepeticao);
    return 0;
}
