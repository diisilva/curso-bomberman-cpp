// Aula 19 — Motor completo dos poderes
//
// A Aula 09 mostrou como cada carta é COLETADA. Aqui estão as funções do
// jogo real (projeto_final/bomberman.cpp) que as USAM:
//   chutarBomba            (E)  empurra a bomba à frente até 4 casas
//   teletransportarJogador (T)  salta para uma casa livre sorteada
//   detonarBombasRelogio   (F)  explode todas as bombas-relógio do dono
//   bombaFereJogador            quem a chama de cada bomba fere (imunidade e modos)
//   podeAndarPara               a carta de atravessar libera só caixas comuns
#include <cstdio>
#include <random>
#include <vector>

const int LINHAS = 5, COLUNAS = 9, ALCANCE_CHUTE = 4, DONO_INIMIGO = 2;
enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL, PAREDE_BONUS };
TipoCelula mapa[LINHAS][COLUNAS];
struct Posicao { int linha, coluna; };
struct Bomba { Posicao posicao; bool ativa = true, explodindo = false, relogio = false; int dono = 0; };
struct Inimigo { Posicao posicao; bool vivo = true; };

std::vector<Bomba> bombas;
std::vector<Inimigo> inimigos;
Posicao jogador{1, 1}, direcaoJogador{0, 1};
float jogadorVisualL = 1, jogadorVisualC = 1;
bool possuiCartaChute = true, possuiCartaTeleporte = true;
bool imuneAPropriaBomba = false, atravessaBlocoQuebravel = false, modoRivais = false;
std::mt19937 gerador(7);   // no jogo, a semente muda a cada partida

bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }
bool livre(int l, int c) { return dentro(l, c) && mapa[l][c] == VAZIO; }
bool temInimigo(Posicao p) { for (auto& i : inimigos) if (i.vivo && iguais(i.posicao, p)) return true; return false; }
bool bombaEm(Posicao p) { for (auto& b : bombas) if (b.ativa && iguais(b.posicao, p)) return true; return false; }

void chutarBomba() {
    if (!possuiCartaChute) return;
    Posicao esperado{jogador.linha + direcaoJogador.linha, jogador.coluna + direcaoJogador.coluna};
    for (Bomba& b : bombas) {
        if (!b.ativa || b.explodindo || !iguais(b.posicao, esperado)) continue;
        Posicao destinoFinal = b.posicao;
        for (int passo = 1; passo <= ALCANCE_CHUTE; passo++) {
            Posicao tentativa{b.posicao.linha + direcaoJogador.linha * passo, b.posicao.coluna + direcaoJogador.coluna * passo};
            if (!livre(tentativa.linha, tentativa.coluna) || temInimigo(tentativa)) break;
            destinoFinal = tentativa;
        }
        b.posicao = destinoFinal;
        possuiCartaChute = false;     // a carta só é gasta num chute que encontrou bomba
        return;
    }
}

void teletransportarJogador() {
    if (!possuiCartaTeleporte) return;
    std::vector<Posicao> livres;
    for (int l = 1; l < LINHAS - 1; l++)
        for (int c = 1; c < COLUNAS - 1; c++)
            if (livre(l, c) && !temInimigo({l, c}) && !bombaEm({l, c})) livres.push_back({l, c});
    if (livres.empty()) return;
    std::uniform_int_distribution<int> sorteio(0, static_cast<int>(livres.size()) - 1);
    jogador = livres[sorteio(gerador)];
    jogadorVisualL = float(jogador.linha);   // o desenho salta junto, sem deslizar pelo mapa
    jogadorVisualC = float(jogador.coluna);
    possuiCartaTeleporte = false;
}

void detonarBombasRelogio() {
    for (Bomba& b : bombas)
        if (b.ativa && !b.explodindo && b.relogio && (b.dono == 0 || !modoRivais)) b.explodindo = true;   // iniciarExplosaoBomba
}

bool bombaFereJogador(const Bomba& b, int numero) {
    if (b.dono == DONO_INIMIGO) return true;
    if (b.dono == numero - 1) return !imuneAPropriaBomba;
    return modoRivais;
}

bool podeAndarPara(Posicao destino) {
    if (bombaEm(destino)) return false;
    return livre(destino.linha, destino.coluna) ||
           (atravessaBlocoQuebravel && dentro(destino.linha, destino.coluna) && mapa[destino.linha][destino.coluna] == PAREDE_FRAGIL);
}

int main() {
    const char* desenho[LINHAS] = {"#########", "#.......#", "#.#.#.#.#", "#....+B.#", "#########"};
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++)
            mapa[l][c] = desenho[l][c] == '#' ? PAREDE_SOLIDA : desenho[l][c] == '+' ? PAREDE_FRAGIL
                       : desenho[l][c] == 'B' ? PAREDE_BONUS : VAZIO;

    // CHUTE: bomba logo à frente, olhando para a direita.
    bombas.push_back({{1, 2}});
    inimigos.push_back({{1, 5}});
    chutarBomba();
    std::printf("chute com inimigo em (1,5): bomba para em (%d,%d); carta=%d\n",
                bombas[0].posicao.linha, bombas[0].posicao.coluna, possuiCartaChute);
    possuiCartaChute = true; inimigos.clear(); bombas[0].posicao = {1, 2};
    chutarBomba();
    std::printf("chute sem inimigo:          bomba para em (%d,%d) (alcance %d)\n",
                bombas[0].posicao.linha, bombas[0].posicao.coluna, ALCANCE_CHUTE);

    // TELEPORTE
    teletransportarJogador();
    std::printf("teleporte: jogador em (%d,%d), desenho em (%.0f,%.0f), carta=%d\n",
                jogador.linha, jogador.coluna, jogadorVisualL, jogadorVisualC, possuiCartaTeleporte);

    // RELÓGIO: duas bombas que não têm fusível; F explode as duas.
    bombas = {{{3, 1}}, {{1, 7}}};
    for (Bomba& b : bombas) b.relogio = true;
    detonarBombasRelogio();
    std::printf("relogio: F detonou %d de 2 bombas\n", bombas[0].explodindo + bombas[1].explodindo);

    // QUEM A CHAMA FERE: jogador 1 diante de bombas de cada dono.
    std::printf("fere P1?  bomba do inimigo / propria / de P2\n");
    for (bool rivais : {false, true})
        for (bool imune : {false, true}) {
            modoRivais = rivais; imuneAPropriaBomba = imune;
            std::printf("  %-8s %-8s %d / %d / %d\n", rivais ? "inimigos" : "amigos", imune ? "imune" : "",
                        bombaFereJogador({{0, 0}, true, false, false, DONO_INIMIGO}, 1),
                        bombaFereJogador({{0, 0}, true, false, false, 0}, 1),
                        bombaFereJogador({{0, 0}, true, false, false, 1}, 1));
        }

    // ATRAVESSAR: só caixa comum; bloco com bônus e parede continuam bloqueando.
    atravessaBlocoQuebravel = true;
    bombas.clear();
    std::printf("atravessa: caixa=%d bloco bonus=%d parede=%d\n",
                podeAndarPara({3, 5}), podeAndarPara({3, 6}), podeAndarPara({2, 2}));
    return 0;
}
