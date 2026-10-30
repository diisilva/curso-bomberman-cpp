// Aula 08 — Bomba e explosão recursiva
//
// O ciclo de vida de uma bomba no jogo real (projeto_final/bomberman.cpp):
//   colocarBombaEm  → fusível de 2200 ms
//   atualizarBombas → desconta o tempo a cada atualizar(8)
//   iniciarExplosaoBomba → chama propagarExplosao nas quatro direções
//   propagarExplosao → recursiva: uma casa por chamada, até o alcance acabar
//   atualizarBombas → a chama dura 650 ms e a bomba sai do vetor
#include <cstdio>
#include <vector>

const int LINHAS = 5, COLUNAS = 7;
const int TEMPO_BOMBA_MS = 2200;
const int TEMPO_EXPLOSAO_MS = 650;
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL };
TipoCelula mapa[LINHAS][COLUNAS];

struct Posicao { int linha, coluna; };
struct Bomba {                         // os campos que importam aqui; o jogo tem mais
    Posicao posicao{0, 0};
    bool ativa = false;
    bool explodindo = false;
    int tempoMs = 0;
    std::vector<Posicao> celulasExplosao;
};
std::vector<Bomba> bombas;
int alcanceExplosao = 3;               // no jogo começa em 1 e a carta RAIO sobe até 5
int caixasDestruidas = 0;

bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }

void colocarBombaEm(Posicao pos) {
    Bomba nova;
    nova.posicao = pos;
    nova.ativa = true;
    nova.tempoMs = TEMPO_BOMBA_MS;
    bombas.push_back(nova);
}

// Caso base 1: alcance acabou. Caso base 2: saiu do mapa ou bateu em parede.
// Caixa NÃO é caso base: ela quebra e a chama segue em frente.
void propagarExplosao(std::vector<Posicao>& celulas, int dl, int dc, Posicao posAtual, int alcanceRestante) {
    if (alcanceRestante <= 0) return;
    Posicao proxima{posAtual.linha + dl, posAtual.coluna + dc};
    if (!dentro(proxima.linha, proxima.coluna) || mapa[proxima.linha][proxima.coluna] == PAREDE_SOLIDA) return;
    celulas.push_back(proxima);
    if (mapa[proxima.linha][proxima.coluna] == PAREDE_FRAGIL) {
        mapa[proxima.linha][proxima.coluna] = VAZIO;
        caixasDestruidas++;
    }
    propagarExplosao(celulas, dl, dc, proxima, alcanceRestante - 1);
}

void iniciarExplosaoBomba(Bomba& b) {
    b.explodindo = true;
    b.tempoMs = TEMPO_EXPLOSAO_MS;       // o mesmo contador passa a medir a chama
    b.celulasExplosao.clear();
    b.celulasExplosao.push_back(b.posicao);
    propagarExplosao(b.celulasExplosao, -1, 0, b.posicao, alcanceExplosao);
    propagarExplosao(b.celulasExplosao, 1, 0, b.posicao, alcanceExplosao);
    propagarExplosao(b.celulasExplosao, 0, -1, b.posicao, alcanceExplosao);
    propagarExplosao(b.celulasExplosao, 0, 1, b.posicao, alcanceExplosao);
}

void atualizarBombas(int tempoMs) {
    for (Bomba& b : bombas) {
        if (!b.ativa || b.explodindo) continue;
        b.tempoMs -= tempoMs;
        if (b.tempoMs <= 0) iniciarExplosaoBomba(b);
    }
    // Apagar dentro de um for normal: só avança i quando NÃO apagou.
    for (size_t i = 0; i < bombas.size();) {
        Bomba& b = bombas[i];
        if (b.explodindo) {
            b.tempoMs -= tempoMs;
            if (b.tempoMs <= 0) { bombas.erase(bombas.begin() + i); continue; }
        }
        i++;
    }
}

void imprimirMapa(const Bomba& b) {
    char tela[LINHAS][COLUNAS + 1] = {};
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++)
            tela[l][c] = mapa[l][c] == PAREDE_SOLIDA ? '#' : mapa[l][c] == PAREDE_FRAGIL ? '+' : '.';
    for (const Posicao& p : b.celulasExplosao) tela[p.linha][p.coluna] = '*';
    tela[b.posicao.linha][b.posicao.coluna] = 'B';
    for (auto& linha : tela) std::printf("  %s\n", linha);
}

int main() {
    const char* desenho[LINHAS] = {"#######", "#.....#", "#.++..#", "#.....#", "#######"};
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++)
            mapa[l][c] = desenho[l][c] == '#' ? PAREDE_SOLIDA : desenho[l][c] == '+' ? PAREDE_FRAGIL : VAZIO;

    colocarBombaEm({2, 1});
    std::printf("t=0     bomba em (2,1), fusivel=%d ms, alcance=%d\n", TEMPO_BOMBA_MS, alcanceExplosao);
    bool mostrou = false;
    for (int t = 8; !bombas.empty(); t += 8) {
        atualizarBombas(8);
        if (!mostrou && !bombas.empty() && bombas[0].explodindo) {
            mostrou = true;
            std::printf("t=%-5d explodiu: %zu celulas com chama\n", t, bombas[0].celulasExplosao.size());
            imprimirMapa(bombas[0]);
        }
        if (bombas.empty()) std::printf("t=%-5d a chama acabou e a bomba saiu do vetor\n", t);
    }
    std::printf("caixas destruidas=%d\n", caixasDestruidas);
    return 0;
}
