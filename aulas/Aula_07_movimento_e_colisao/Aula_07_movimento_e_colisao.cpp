// Aula 07 — Movimento e colisão
//
// moverJogador, podeAndarPara, visualNoDestino e avancarVisual seguem o jogo
// real (projeto_final/bomberman.cpp). O mapa é pequeno e não há bombas nem
// inimigos, mas as regras de quando um passo é aceito são as mesmas:
//   - o destino precisa estar livre (colisão);
//   - o passo anterior precisa ter terminado (cooldown de 280 ms);
//   - o desenho precisa ter chegado ao centro da casa (curvas só no centro).
#include <algorithm>
#include <cmath>
#include <cstdio>

const int LINHAS = 5, COLUNAS = 5;
const int INTERVALO_MOVIMENTO_MS = 280;
const float VELOCIDADE_VISUAL_CELS_SEG = 3.6f;
const char* mapa[LINHAS] = {"#####",
                            "#...#",
                            "#.#.#",    // (2,2) é um pilar
                            "#...#",
                            "#####"};

struct Posicao { int linha, coluna; };
Posicao jogador{1, 1};                    // casa lógica: reservada na hora
float jogadorVisualL = 1, jogadorVisualC = 1;   // desenho: chega aos poucos
int tempoMovimentoMs = 0;

bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }
bool livre(int l, int c) { return dentro(l, c) && mapa[l][c] == '.'; }
bool podeAndarPara(Posicao destino) { return livre(destino.linha, destino.coluna); }   // no jogo: + bombas e chefe

bool visualNoDestino(float vl, float vc, Posicao alvo) {
    return std::fabs(vl - alvo.linha) < 0.0001f && std::fabs(vc - alvo.coluna) < 0.0001f;
}

float avancarVisual(float& vl, float& vc, Posicao alvo, int tempoMs) {
    float dl = alvo.linha - vl, dc = alvo.coluna - vc;
    float distancia = std::sqrt(dl * dl + dc * dc);
    if (distancia < 0.0001f) { vl = float(alvo.linha); vc = float(alvo.coluna); return 0; }
    float passo = std::min(distancia, VELOCIDADE_VISUAL_CELS_SEG * tempoMs / 1000.0f);
    vl += dl / distancia * passo;
    vc += dc / distancia * passo;
    return passo;
}

// As três guardas do começo são as mesmas do jogo (lá há também estado e queda).
void moverJogador(int dl, int dc) {
    if (tempoMovimentoMs > 0 || !visualNoDestino(jogadorVisualL, jogadorVisualC, jogador)) return;
    Posicao origem = jogador;
    Posicao destino{jogador.linha + dl, jogador.coluna + dc};
    if (podeAndarPara(destino)) jogador = destino;
    tempoMovimentoMs = iguais(origem, jogador) ? 0 : INTERVALO_MOVIMENTO_MS;
}

int relogio = 0;
void atualizar(int tempoMs) {
    relogio += tempoMs;
    tempoMovimentoMs = std::max(0, tempoMovimentoMs - tempoMs);
    avancarVisual(jogadorVisualL, jogadorVisualC, jogador, tempoMs);
}
void esperar(int ms) { for (int t = 0; t < ms; t += 8) atualizar(8); }

void mostrar(const char* acao) {
    std::printf("t=%3d %-34s logica=(%d,%d) visual=(%.2f,%.2f) chegou=%d\n", relogio, acao,
                jogador.linha, jogador.coluna, jogadorVisualL, jogadorVisualC,
                visualNoDestino(jogadorVisualL, jogadorVisualC, jogador));
}

int main() {
    moverJogador(0, 1);  mostrar("direita: reserva (1,2)");
    esperar(80);         mostrar("80 ms depois");
    moverJogador(1, 0);  mostrar("baixo no meio do passo: ignorado");
    esperar(200);        mostrar("mais 200 ms: chegou ao centro");
    moverJogador(1, 0);  mostrar("baixo: (2,2) e pilar, recusado");
    moverJogador(0, 1);  mostrar("direita: reserva (1,3)");
    return 0;
}
