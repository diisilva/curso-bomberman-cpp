// Aula 11 — Perigo e fuga do bot
//
// O bot do modo demonstração precisa PREVER onde a chama vai chegar antes de
// plantar uma bomba ou escolher para onde fugir. Para isso o jogo usa
// celulaNoAlcance. A chama de verdade é calculada por propagarExplosao (Aula 08).
// As duas funções abaixo são copiadas do jogo; compare as respostas.
#include <cstdio>
#include <cstdlib>
#include <vector>

const int LINHAS = 3, COLUNAS = 7;
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL, PAREDE_BONUS };
TipoCelula mapa[LINHAS][COLUNAS];
struct Posicao { int linha, coluna; };
bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }

// PREVISÃO (bot): andando da bomba até p, na primeira caixa a resposta é
// "só se p for a própria caixa". Atrás dela, a previsão diz que é seguro.
bool celulaNoAlcance(Posicao origemBomba, Posicao p, int alcance) {
    if (iguais(p, origemBomba)) return true;
    int dl = p.linha - origemBomba.linha;
    int dc = p.coluna - origemBomba.coluna;
    if (dl != 0 && dc != 0) return false;
    int distancia = std::abs(dl) + std::abs(dc);
    if (distancia > alcance) return false;
    int passoL = (dl > 0) - (dl < 0);     // -1, 0 ou +1: o sinal de dl
    int passoC = (dc > 0) - (dc < 0);
    for (int i = 1; i <= distancia; i++) {
        int l = origemBomba.linha + passoL * i;
        int c = origemBomba.coluna + passoC * i;
        if (mapa[l][c] == PAREDE_SOLIDA) return false;
        if (mapa[l][c] == PAREDE_FRAGIL || mapa[l][c] == PAREDE_BONUS) return i == distancia;
    }
    return true;
}

// REALIDADE (explosão): a caixa quebra e a chama continua (Aula 08).
// Aqui ela só anota as células, sem quebrar nada, para não mexer no mapa.
void propagarExplosao(std::vector<Posicao>& celulas, int dl, int dc, Posicao posAtual, int alcanceRestante) {
    if (alcanceRestante <= 0) return;
    Posicao proxima{posAtual.linha + dl, posAtual.coluna + dc};
    if (!dentro(proxima.linha, proxima.coluna) || mapa[proxima.linha][proxima.coluna] == PAREDE_SOLIDA) return;
    celulas.push_back(proxima);
    propagarExplosao(celulas, dl, dc, proxima, alcanceRestante - 1);
}

int main() {
    const char* linha = "#.++..#";   // bomba em (1,1); caixas em (1,2) e (1,3)
    for (int c = 0; c < COLUNAS; c++) {
        mapa[0][c] = mapa[2][c] = PAREDE_SOLIDA;
        mapa[1][c] = linha[c] == '#' ? PAREDE_SOLIDA : linha[c] == '+' ? PAREDE_FRAGIL : VAZIO;
    }
    Posicao bomba{1, 1};
    const int alcance = 4;
    std::vector<Posicao> chama{bomba};
    propagarExplosao(chama, 0, 1, bomba, alcance);

    std::printf("linha %s, bomba em (1,1), alcance %d\n", linha, alcance);
    for (int c = 2; c <= 5; c++) {
        Posicao alvo{1, c};
        bool prevista = celulaNoAlcance(bomba, alvo, alcance);
        bool real = false;
        for (const Posicao& p : chama) real = real || iguais(p, alvo);
        std::printf("(1,%d) %c prevista=%d real=%d %s\n", c, linha[c], prevista, real,
                    prevista != real ? "<- o bot acha seguro, mas a chama chega" : "");
    }
    return 0;
}
