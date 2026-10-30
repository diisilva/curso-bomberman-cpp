// Aula 08 — solução do exercício 3: reação em cadeia.
//
// No jogo real, a chama de uma bomba passa por cima de outra bomba sem
// detoná-la: cada uma espera o próprio fusível. Esta versão acrescenta a
// regra clássica do Bomberman: bomba atingida por chama explode na hora.
// A detonação também é recursiva: B2 pode detonar B3, que detona B4...
#include <cstdio>
#include <vector>

const int LINHAS = 5, COLUNAS = 7;
const int TEMPO_BOMBA_MS = 2200;
const int TEMPO_EXPLOSAO_MS = 650;
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL };
TipoCelula mapa[LINHAS][COLUNAS];

struct Posicao { int linha, coluna; };
struct Bomba {
    Posicao posicao{0, 0};
    bool ativa = false;
    bool explodindo = false;
    int tempoMs = 0;
    int explodiuEm = -1;   // só para o relatório
    std::vector<Posicao> celulasExplosao;
};
std::vector<Bomba> bombas;
int alcanceExplosao = 3;
int relogio = 0;
bool emCadeia = true;

bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }
bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }

void colocarBombaEm(Posicao pos) {
    Bomba nova;
    nova.posicao = pos;
    nova.ativa = true;
    nova.tempoMs = TEMPO_BOMBA_MS;
    bombas.push_back(nova);
}

void propagarExplosao(std::vector<Posicao>& celulas, int dl, int dc, Posicao posAtual, int alcanceRestante) {
    if (alcanceRestante <= 0) return;
    Posicao proxima{posAtual.linha + dl, posAtual.coluna + dc};
    if (!dentro(proxima.linha, proxima.coluna) || mapa[proxima.linha][proxima.coluna] == PAREDE_SOLIDA) return;
    celulas.push_back(proxima);
    if (mapa[proxima.linha][proxima.coluna] == PAREDE_FRAGIL) mapa[proxima.linha][proxima.coluna] = VAZIO;
    propagarExplosao(celulas, dl, dc, proxima, alcanceRestante - 1);
}

void iniciarExplosaoBomba(Bomba& b) {
    b.explodindo = true;
    b.explodiuEm = relogio;
    b.tempoMs = TEMPO_EXPLOSAO_MS;
    b.celulasExplosao.clear();
    b.celulasExplosao.push_back(b.posicao);
    propagarExplosao(b.celulasExplosao, -1, 0, b.posicao, alcanceExplosao);
    propagarExplosao(b.celulasExplosao, 1, 0, b.posicao, alcanceExplosao);
    propagarExplosao(b.celulasExplosao, 0, -1, b.posicao, alcanceExplosao);
    propagarExplosao(b.celulasExplosao, 0, 1, b.posicao, alcanceExplosao);
    if (!emCadeia) return;
    // NOVO: toda bomba ainda armada dentro da chama explode agora.
    // Copiamos as células porque a chamada recursiva mexe em outras bombas.
    std::vector<Posicao> chama = b.celulasExplosao;
    for (Bomba& outra : bombas)
        if (!outra.explodindo)
            for (const Posicao& p : chama)
                if (iguais(p, outra.posicao)) { iniciarExplosaoBomba(outra); break; }
}

void atualizarBombas(int tempoMs) {
    for (Bomba& b : bombas) {
        if (!b.ativa || b.explodindo) continue;
        b.tempoMs -= tempoMs;
        if (b.tempoMs <= 0) iniciarExplosaoBomba(b);
    }
    // Aqui as bombas NÃO são apagadas, para o relatório final ler explodiuEm.
    for (Bomba& b : bombas) if (b.explodindo && b.tempoMs > 0) b.tempoMs -= tempoMs;
}

void rodar(bool cadeia) {
    const char* desenho[LINHAS] = {"#######", "#.....#", "#.++..#", "#.....#", "#######"};
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++)
            mapa[l][c] = desenho[l][c] == '#' ? PAREDE_SOLIDA : desenho[l][c] == '+' ? PAREDE_FRAGIL : VAZIO;
    bombas.clear();
    relogio = 0;
    emCadeia = cadeia;
    colocarBombaEm({2, 1});                         // B1 em t=0
    for (relogio = 8; relogio <= 4000; relogio += 8) {
        atualizarBombas(8);
        if (relogio == 1000) colocarBombaEm({2, 4}); // B2 em t=1000, no caminho da chama de B1
    }
    std::printf("%s: B1 explodiu em t=%d, B2 explodiu em t=%d\n", cadeia ? "com cadeia" : "jogo real ",
                bombas[0].explodiuEm, bombas[1].explodiuEm);
}

int main() {
    rodar(false);
    rodar(true);
    return 0;
}
