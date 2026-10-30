// Aula 02 — Matriz, enum e mapa
//
// criarMapa e limparArea são as funções reais de projeto_final/bomberman.cpp,
// com a mesma arena de 13 x 15 e a mesma chance de 36% de caixa.
// A única diferença: a semente do sorteio é fixa (2026), para a saída
// ser igual em todas as execuções e você poder conferir sua previsão.
#include <iostream>
#include <random>

const int LINHAS = 13;
const int COLUNAS = 15;

// Mesmo enum do jogo: cada célula guarda UM destes estados.
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL, PAREDE_BONUS, PORTAL };

TipoCelula mapa[LINHAS][COLUNAS];   // primeiro índice = linha, segundo = coluna
std::mt19937 gerador(2026);         // no jogo: mt19937 gerador(random_device{}());

void criarMapa() {
    std::uniform_int_distribution<int> chance(0, 99);
    for (int l = 0; l < LINHAS; l++) {
        for (int c = 0; c < COLUNAS; c++) {
            bool borda = l == 0 || l == LINHAS - 1 || c == 0 || c == COLUNAS - 1;
            bool pilar = l % 2 == 0 && c % 2 == 0;   // um pilar a cada duas casas
            if (borda || pilar) mapa[l][c] = PAREDE_SOLIDA;
            else if (chance(gerador) < 36) mapa[l][c] = PAREDE_FRAGIL;
            else mapa[l][c] = VAZIO;
        }
    }
}

// Abre uma cruz em volta do ponto de partida: sem isso, o jogador poderia
// nascer cercado de caixas e morrer na primeira bomba.
void limparArea(int linha, int coluna) {
    mapa[linha][coluna] = VAZIO;
    if (linha > 0) mapa[linha - 1][coluna] = VAZIO;
    if (linha < LINHAS - 1) mapa[linha + 1][coluna] = VAZIO;
    if (coluna > 0) mapa[linha][coluna - 1] = VAZIO;
    if (coluna < COLUNAS - 1) mapa[linha][coluna + 1] = VAZIO;
}

// O enum é a regra; o caractere é só um jeito de mostrar a regra no console.
char simbolo(TipoCelula t) {
    switch (t) {
        case PAREDE_SOLIDA: return '#';
        case PAREDE_FRAGIL: return '+';
        case PAREDE_BONUS:  return 'B';
        case PORTAL:        return 'P';
        default:            return '.';
    }
}

// A borda inteira deve continuar sendo parede: é ela que fecha a arena.
bool bordaIntacta() {
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++) {
            bool borda = l == 0 || l == LINHAS - 1 || c == 0 || c == COLUNAS - 1;
            if (borda && mapa[l][c] != PAREDE_SOLIDA) return false;
        }
    return true;
}

int main() {
    criarMapa();
    limparArea(1, 1);
    int caixas = 0;
    for (int l = 0; l < LINHAS; l++) {
        for (int c = 0; c < COLUNAS; c++) {
            std::cout << simbolo(mapa[l][c]);
            if (mapa[l][c] == PAREDE_FRAGIL) caixas++;
        }
        std::cout << "\n";
    }
    // 143 casas internas - 30 pilares = 113 sorteadas; 36% de 113 dá cerca de 41.
    std::cout << "caixas=" << caixas << " (esperado perto de 41)\n";
    std::cout << "borda intacta=" << bordaIntacta() << "\n";   // confira com o mapa acima!
    return 0;
}
