// Aula 19 — solução do exercício 2: chute contra a parede.
//
// No jogo, chutarBomba gasta a carta assim que encontra a bomba à frente,
// mesmo que ela não ande nenhuma casa (parede ou inimigo logo atrás).
// A variação "justa" só gasta a carta se a bomba realmente se moveu.
#include <cstdio>
#include <vector>

const int ALCANCE_CHUTE = 4;
struct Posicao { int linha, coluna; };
struct Bomba { Posicao posicao; };
const char* corredor = "#.B#....#";   // linha 1: jogador em c=1, bomba em c=2, parede em c=3

bool livre(int c) { return corredor[c] == '.'; }

bool chutar(Bomba& b, bool& carta, bool justo) {
    if (!carta) return false;
    int destino = b.posicao.coluna;
    for (int passo = 1; passo <= ALCANCE_CHUTE; passo++) {
        if (!livre(b.posicao.coluna + passo)) break;
        destino = b.posicao.coluna + passo;
    }
    bool andou = destino != b.posicao.coluna;
    b.posicao.coluna = destino;
    if (!justo || andou) carta = false;   // jogo: sempre gasta; variação: só se andou
    return andou;
}

int main() {
    for (bool justo : {false, true}) {
        Bomba b{{1, 2}};
        bool carta = true;
        bool andou = chutar(b, carta, justo);
        std::printf("%s: bomba andou=%d, ficou em c=%d, ainda tem a carta=%d\n",
                    justo ? "variacao justa" : "regra do jogo ", andou, b.posicao.coluna, carta);
    }
    return 0;
}
