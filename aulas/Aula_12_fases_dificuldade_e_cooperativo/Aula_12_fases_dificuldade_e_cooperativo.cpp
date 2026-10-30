// Aula 12 — Fases, dificuldade e dois jogadores
//
// As três funções que trocam de fase no jogo real e o que cada uma preserva:
//   novaPartida   → zera TUDO (pontos, vidas, estatísticas) e monta a fase
//   reiniciarFase → morreu com vidas: só remonta a fase
//   avancarFase   → passou pelo portal: fase+1, vidas voltam a 3, contadores da fase zeram
// Mais: quantos inimigos cada dificuldade cria, quantos soltam bomba e como
// os pontos se dividem quando jogam dois.
#include <algorithm>
#include <cstdio>

enum Dificuldade { FACIL, MEDIO, DIFICIL };
const int VIDAS_BASE = 3, TOTAL_FASES = 10, ALCANCE_EXPLOSAO_INICIAL = 1;
const int PONTOS_POR_INIMIGO = 100, PONTOS_POR_FASE = 500, CUSTO_POR_MOVIMENTO = 1, CUSTO_POR_BOMBA = 5;

struct EstatisticasPartida { int movimentos = 0, bombasUsadas = 0, movimentosFase = 0, bombasFase = 0; };

int faseAtual = 1, faseSelecionada = 1, vidas = VIDAS_BASE, pontos = 0;
int pontosJogador[2] = {};
int alcanceExplosao = ALCANCE_EXPLOSAO_INICIAL;
EstatisticasPartida estatisticas;
Dificuldade dificuldade = FACIL;

void montarFase() {                        // no jogo: mapa, inimigos, chefe, Mario...
    alcanceExplosao = ALCANCE_EXPLOSAO_INICIAL;   // bônus da fase zeram (Aula 09)
}
void reiniciarFase() { montarFase(); }
void avancarFase() {
    if (faseAtual < TOTAL_FASES) faseAtual++;
    vidas = VIDAS_BASE;
    estatisticas.movimentosFase = estatisticas.bombasFase = 0;
    montarFase();
}
void novaPartida(Dificuldade novaDificuldade) {
    dificuldade = novaDificuldade;
    faseAtual = faseSelecionada;
    pontos = 0;
    pontosJogador[0] = pontosJogador[1] = 0;
    vidas = VIDAS_BASE;
    estatisticas = EstatisticasPartida{};
    montarFase();
}

// Único lugar que mexe em "pontos": nunca negativo; dono 0/1 também credita a parte do jogador.
void ajustarPontos(int delta, int dono = -1) {
    pontos = std::max(0, pontos + delta);
    if (dono == 0 || dono == 1) pontosJogador[dono] = std::max(0, pontosJogador[dono] + delta);
}

int quantidadeInimigosNormais() {
    switch (dificuldade) {
        case FACIL: return 3;
        case MEDIO: return 5;
        case DIFICIL: return 7;
    }
    return 3;
}
int inimigosComBomba(int total) {          // mesmo cálculo de montarFase
    int percentual = dificuldade == MEDIO ? 50 : 75;
    return dificuldade == FACIL ? std::min(1, total) : (total * percentual + 50) / 100;
}

// Simula um trecho de jogo: andar, plantar e (opcionalmente) ganhar pontos.
void jogar(int movimentos, int bombasUsadas, int pontosGanhos, int dono = 0) {
    estatisticas.movimentos += movimentos;   estatisticas.movimentosFase += movimentos;
    estatisticas.bombasUsadas += bombasUsadas; estatisticas.bombasFase += bombasUsadas;
    ajustarPontos(pontosGanhos, dono);
    alcanceExplosao = 2;                       // pegou uma carta RAIO no caminho
}
void entrarNoPortal() {                      // avancarOuVencerSePortal
    int bonus = std::max(0, PONTOS_POR_FASE - estatisticas.movimentosFase * CUSTO_POR_MOVIMENTO
                                            - estatisticas.bombasFase * CUSTO_POR_BOMBA);
    std::printf("  bonus do portal = max(0, 500 - %d - 5 x %d) = %d\n",
                estatisticas.movimentosFase, estatisticas.bombasFase, bonus);
    ajustarPontos(bonus);
    avancarFase();
}

void mostrar(const char* momento) {
    std::printf("%-28s fase=%d vidas=%d pontos=%-4d alcance=%d movFase=%-3d bombasFase=%d\n", momento,
                faseAtual, vidas, pontos, alcanceExplosao, estatisticas.movimentosFase, estatisticas.bombasFase);
}

int main() {
    novaPartida(MEDIO);                  mostrar("novaPartida");
    jogar(40, 6, 2 * PONTOS_POR_INIMIGO); mostrar("jogou: 40 passos, 6 bombas");
    vidas--; reiniciarFase();            mostrar("morreu -> reiniciarFase");
    jogar(30, 4, PONTOS_POR_INIMIGO);    mostrar("jogou: 30 passos, 4 bombas");
    entrarNoPortal();                    mostrar("portal -> avancarFase");

    std::printf("\ninimigos por dificuldade (e quantos soltam bomba):\n");
    for (Dificuldade d : {FACIL, MEDIO, DIFICIL}) {
        dificuldade = d;
        int total = quantidadeInimigosNormais();
        std::printf("  %-7s %d inimigos, %d com bomba\n", d == FACIL ? "FACIL" : d == MEDIO ? "MEDIO" : "DIFICIL",
                    total, inimigosComBomba(total));
    }

    novaPartida(MEDIO);
    ajustarPontos(PONTOS_POR_INIMIGO, 0);   // P1 derrubou um inimigo
    ajustarPontos(20, 1);                   // P2 quebrou uma caixa
    std::printf("\ndois jogadores: total=%d P1=%d P2=%d\n", pontos, pontosJogador[0], pontosJogador[1]);
    return 0;
}
