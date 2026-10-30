// Aula 01 — Tipos, funções e referências
//
// Posicao, iguais e avancarVisual existem no jogo real (projeto_final/bomberman.cpp).
// Aqui elas aparecem quase iguais, só sem o resto do jogo em volta.
#include <algorithm>
#include <cmath>
#include <iostream>

// No jogo: struct Posicao { int linha, coluna; };
// A struct junta duas variáveis que sempre andam juntas: a célula da matriz.
struct Posicao { int linha, coluna; };

// No jogo, o movimento suave do desenho anda 3,6 células por segundo.
const float VELOCIDADE_VISUAL_CELS_SEG = 3.6f;

// Por valor: a função só LÊ as posições, então cópias pequenas bastam.
bool iguais(Posicao a, Posicao b) {
    return a.linha == b.linha && a.coluna == b.coluna;
}

// Por valor, mas tentando alterar: muda só a cópia local, que some no fim da função.
void moverErrado(Posicao alvo, int dl, int dc) {
    alvo.linha += dl;
    alvo.coluna += dc;
}

// Por referência (&): "alvo" é outro nome para a variável de quem chamou.
void mover(Posicao& alvo, int dl, int dc) {
    alvo.linha += dl;
    alvo.coluna += dc;
}

// Mesmo corpo de avancarVisual do jogo. As duas referências float são a
// "saída" da função; o retorno informa quanto o desenho andou neste passo.
float avancarVisual(float& visualL, float& visualC, Posicao alvo, int tempoMs) {
    float dl = static_cast<float>(alvo.linha) - visualL;
    float dc = static_cast<float>(alvo.coluna) - visualC;
    float distancia = std::sqrt(dl * dl + dc * dc);
    if (distancia < 0.0001f) { visualL = float(alvo.linha); visualC = float(alvo.coluna); return 0; }
    float passo = std::min(distancia, VELOCIDADE_VISUAL_CELS_SEG * tempoMs / 1000.0f);
    visualL += dl / distancia * passo;
    visualC += dc / distancia * passo;
    return passo;
}

int main() {
    Posicao jogador{1, 1};
    float visualL = 1, visualC = 1;  // onde o sprite está desenhado

    moverErrado(jogador, 0, 1);
    std::cout << "moverErrado -> jogador=(" << jogador.linha << "," << jogador.coluna << ")\n";

    mover(jogador, 0, 1);
    std::cout << "mover       -> jogador=(" << jogador.linha << "," << jogador.coluna << ")\n";

    Posicao destino{1, 2};
    std::cout << "iguais(jogador, destino)=" << iguais(jogador, destino) << "\n";

    // A célula lógica já é (1,2), mas o desenho chega lá aos poucos.
    float andou = avancarVisual(visualL, visualC, jogador, 100);
    std::cout << "apos 100 ms -> visual=(" << visualL << "," << visualC << ") andou=" << andou << "\n";
    return 0;
}
