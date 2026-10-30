// Aula 11 — solução do exercício 2: uma previsão que concorda com a chama.
//
// propagarExplosao só para em PAREDE_SOLIDA, no fim do mapa ou quando o
// alcance acaba. A previsão corrigida segue a mesma regra: caixa não para nada.
// O teste compara previsão e chama em TODAS as casas, para vários alcances.
#include <cstdio>
#include <cstdlib>
#include <vector>

const int LINHAS = 5, COLUNAS = 9;
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL, PAREDE_BONUS };
TipoCelula mapa[LINHAS][COLUNAS];
struct Posicao { int linha, coluna; };
bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }

bool celulaNoAlcanceCorrigida(Posicao origemBomba, Posicao p, int alcance) {
    if (iguais(p, origemBomba)) return true;
    int dl = p.linha - origemBomba.linha, dc = p.coluna - origemBomba.coluna;
    if (dl != 0 && dc != 0) return false;
    int distancia = std::abs(dl) + std::abs(dc);
    if (distancia > alcance) return false;
    int passoL = (dl > 0) - (dl < 0), passoC = (dc > 0) - (dc < 0);
    for (int i = 1; i <= distancia; i++)
        if (mapa[origemBomba.linha + passoL * i][origemBomba.coluna + passoC * i] == PAREDE_SOLIDA) return false;
    return true;   // MUDOU: caixa e bônus não interrompem mais a previsão
}

void propagarExplosao(std::vector<Posicao>& celulas, int dl, int dc, Posicao posAtual, int alcanceRestante) {
    if (alcanceRestante <= 0) return;
    Posicao proxima{posAtual.linha + dl, posAtual.coluna + dc};
    if (!dentro(proxima.linha, proxima.coluna) || mapa[proxima.linha][proxima.coluna] == PAREDE_SOLIDA) return;
    celulas.push_back(proxima);
    propagarExplosao(celulas, dl, dc, proxima, alcanceRestante - 1);
}

int main() {
    const char* desenho[LINHAS] = {"#########",
                                   "#.++..+.#",
                                   "#+#.#.#.#",
                                   "#...+...#",
                                   "#########"};
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++)
            mapa[l][c] = desenho[l][c] == '#' ? PAREDE_SOLIDA : desenho[l][c] == '+' ? PAREDE_FRAGIL : VAZIO;

    int divergencias = 0, comparacoes = 0;
    for (int alcance = 1; alcance <= 5; alcance++)
        for (int bl = 1; bl < LINHAS - 1; bl++)
            for (int bc = 1; bc < COLUNAS - 1; bc++) {
                if (mapa[bl][bc] != VAZIO) continue;   // bomba só em chão
                Posicao bomba{bl, bc};
                std::vector<Posicao> chama{bomba};
                for (auto d : {std::make_pair(-1, 0), std::make_pair(1, 0), std::make_pair(0, -1), std::make_pair(0, 1)})
                    propagarExplosao(chama, d.first, d.second, bomba, alcance);
                for (int l = 0; l < LINHAS; l++)
                    for (int c = 0; c < COLUNAS; c++) {
                        bool real = false;
                        for (const Posicao& p : chama) real = real || iguais(p, {l, c});
                        bool prevista = celulaNoAlcanceCorrigida(bomba, {l, c}, alcance);
                        comparacoes++;
                        if (real != prevista) divergencias++;
                    }
            }
    std::printf("comparacoes=%d divergencias=%d\n", comparacoes, divergencias);
    return divergencias == 0 ? 0 : 1;
}
