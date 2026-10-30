// Aula 02 — solução do exercício 2: limparArea sem furar a borda nem os pilares.
//
// No jogo, limparArea troca por VAZIO as quatro vizinhas da posição inicial,
// sem olhar o que havia lá. Como todos os nascimentos ficam encostados na
// borda — (1,1), (6,1), (11,13)... — a parede externa ganha buracos, e em
// (6,1) e (6,13) um pilar também some. A correção: só abrir CAIXAS.
#include <iostream>
#include <random>

const int LINHAS = 13;
const int COLUNAS = 15;
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL, PAREDE_BONUS, PORTAL };
TipoCelula mapa[LINHAS][COLUNAS];
std::mt19937 gerador(2026);

struct Posicao { int linha, coluna; };

void criarMapa() {
    std::uniform_int_distribution<int> chance(0, 99);
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++) {
            bool borda = l == 0 || l == LINHAS - 1 || c == 0 || c == COLUNAS - 1;
            bool pilar = l % 2 == 0 && c % 2 == 0;
            if (borda || pilar) mapa[l][c] = PAREDE_SOLIDA;
            else if (chance(gerador) < 36) mapa[l][c] = PAREDE_FRAGIL;
            else mapa[l][c] = VAZIO;
        }
}

// Corrigida: a posição inicial vira chão; das vizinhas, só as caixas saem.
void abrirSeForCaixa(int linha, int coluna) {
    if (mapa[linha][coluna] == PAREDE_FRAGIL) mapa[linha][coluna] = VAZIO;
}
void limparArea(int linha, int coluna) {
    mapa[linha][coluna] = VAZIO;
    abrirSeForCaixa(linha - 1, coluna);   // a posição inicial nunca está na borda,
    abrirSeForCaixa(linha + 1, coluna);   // então as quatro vizinhas existem na matriz
    abrirSeForCaixa(linha, coluna - 1);
    abrirSeForCaixa(linha, coluna + 1);
}

bool bordaIntacta() {
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++) {
            bool borda = l == 0 || l == LINHAS - 1 || c == 0 || c == COLUNAS - 1;
            if (borda && mapa[l][c] != PAREDE_SOLIDA) return false;
        }
    return true;
}

bool pilaresIntactos() {
    for (int l = 2; l < LINHAS - 1; l += 2)
        for (int c = 2; c < COLUNAS - 1; c += 2)
            if (mapa[l][c] != PAREDE_SOLIDA) return false;
    return true;
}

int main() {
    criarMapa();
    // As oito posições de sortearPosicoesIniciais: cantos e meio das bordas.
    const Posicao inicios[] = {{1, 1}, {1, COLUNAS - 2}, {LINHAS - 2, 1}, {LINHAS - 2, COLUNAS - 2},
                               {1, COLUNAS / 2}, {LINHAS - 2, COLUNAS / 2},
                               {LINHAS / 2, 1}, {LINHAS / 2, COLUNAS - 2}};
    for (const Posicao& p : inicios) limparArea(p.linha, p.coluna);
    for (int l = 0; l < LINHAS; l++) {
        for (int c = 0; c < COLUNAS; c++)
            std::cout << (mapa[l][c] == PAREDE_SOLIDA ? '#' : mapa[l][c] == PAREDE_FRAGIL ? '+' : '.');
        std::cout << "\n";
    }
    std::cout << "borda intacta=" << bordaIntacta() << " pilares intactos=" << pilaresIntactos() << "\n";
    return 0;
}
