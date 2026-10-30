// Aula 07 — solução do exercício 2: o toque usa a posição DESENHADA.
//
// No jogo, personagensEmContato compara posições visuais com uma "caixa"
// de meia célula (MEIA_CAIXA_PERSONAGEM = 0,25 de cada lado). Se o toque
// usasse a casa lógica, o jogador morreria no instante em que reserva a casa
// do inimigo, ainda longe dele na tela. Aqui medimos quando cada regra dispara.
#include <algorithm>
#include <cmath>
#include <cstdio>

struct Posicao { int linha, coluna; };
const float VELOCIDADE_VISUAL_CELS_SEG = 3.6f;
const float MEIA_CAIXA_PERSONAGEM = .25f;

bool personagensEmContato(float l1, float c1, float l2, float c2) {
    return std::fabs(l1 - l2) < 2 * MEIA_CAIXA_PERSONAGEM && std::fabs(c1 - c2) < 2 * MEIA_CAIXA_PERSONAGEM;
}

int main() {
    Posicao jogador{1, 2};                 // já reservou a casa (1,2)...
    float visualL = 1, visualC = 1;        // ...mas o desenho ainda está em (1,1)
    const float inimigoL = 1, inimigoC = 2.3f;   // inimigo parado um pouco depois do centro de (1,2)

    int toqueLogico = -1, toqueVisual = -1;
    for (int t = 0; t <= 400; t += 8) {
        bool logico = jogador.linha == 1 && jogador.coluna == 2 && inimigoC >= 2 && inimigoC < 3;  // "mesma casa"
        bool visual = personagensEmContato(visualL, visualC, inimigoL, inimigoC);
        if (logico && toqueLogico < 0) toqueLogico = t;
        if (visual && toqueVisual < 0) toqueVisual = t;
        float passo = std::min(jogador.coluna - visualC, VELOCIDADE_VISUAL_CELS_SEG * 8 / 1000.0f);
        visualC += passo;
    }
    std::printf("toque pela casa logica: t=%d ms (o sprite ainda estava em c=1.00)\n", toqueLogico);
    std::printf("toque pela posicao visual: t=%d ms (distancia < %.2f celula)\n", toqueVisual, 2 * MEIA_CAIXA_PERSONAGEM);
    return 0;
}
