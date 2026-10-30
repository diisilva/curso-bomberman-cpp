// Aula 12 — solução do exercício 2: o bônus do portal depois de uma morte.
//
// No jogo, reiniciarFase não zera movimentosFase nem bombasFase: os passos
// da tentativa em que você morreu continuam descontando do bônus. A variação
// "zerarAoReiniciar" mostra a regra alternativa. Nenhuma das duas está
// errada: é uma decisão de design (punir a morte, ou não).
#include <algorithm>
#include <cstdio>

const int PONTOS_POR_FASE = 500, CUSTO_POR_MOVIMENTO = 1, CUSTO_POR_BOMBA = 5;
struct EstatisticasPartida { int movimentosFase = 0, bombasFase = 0; };

int bonusDoPortal(const EstatisticasPartida& e) {
    return std::max(0, PONTOS_POR_FASE - e.movimentosFase * CUSTO_POR_MOVIMENTO - e.bombasFase * CUSTO_POR_BOMBA);
}

int simular(bool zerarAoReiniciar) {
    EstatisticasPartida e;
    e.movimentosFase += 40; e.bombasFase += 6;                // 1ª tentativa
    if (zerarAoReiniciar) e = EstatisticasPartida{};          // morreu: reiniciarFase
    e.movimentosFase += 30; e.bombasFase += 4;                // 2ª tentativa, até o portal
    return bonusDoPortal(e);
}

int main() {
    std::printf("regra do jogo (acumula as tentativas): bonus=%d\n", simular(false));
    std::printf("variacao (zera ao reiniciar):          bonus=%d\n", simular(true));
    // Quanto custa morrer? Na regra do jogo, a tentativa perdida custou
    // 40 passos + 6 bombas = 70 pontos de bônus, além da vida.
    std::printf("custo da tentativa perdida: %d pontos de bonus\n", simular(true) - simular(false));
    return 0;
}
