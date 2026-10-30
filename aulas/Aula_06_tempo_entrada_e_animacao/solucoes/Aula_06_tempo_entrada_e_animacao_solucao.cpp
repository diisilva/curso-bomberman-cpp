// Aula 06 — solução do exercício 2: o fusível não depende do ritmo do timer.
//
// TEMPO_BOMBA_MS = 2200 no jogo. Com o passo fixo, a bomba explode sempre
// na atualização 2200 / 8 = 275, chegue o WM_TIMER de 16 em 16 ms, em
// intervalos irregulares ou depois de uma pausa. O que muda é o tempo REAL:
// uma pausa acima de 100 ms é cortada pelo teto, e o jogo "congela" junto.
#include <algorithm>
#include <iostream>
#include <vector>

const int TEMPO_BOMBA_MS = 2200;

struct Resultado { int atualizacoes, tempoRealMs; };

Resultado simular(const std::vector<int>& padraoDeEventos) {
    int fusivel = TEMPO_BOMBA_MS, acumulado = 0, atualizacoes = 0, tempoReal = 0;
    for (std::size_t i = 0; fusivel > 0; ++i) {
        int decorrido = padraoDeEventos[i % padraoDeEventos.size()];   // o padrão se repete
        tempoReal += decorrido;
        acumulado += std::min(decorrido, 100);
        while (acumulado >= 8 && fusivel > 0) {
            fusivel -= 8;          // atualizar(8): o fusível só anda aqui
            ++atualizacoes;
            acumulado -= 8;
        }
    }
    return {atualizacoes, tempoReal};
}

int main() {
    struct Caso { const char* nome; std::vector<int> eventos; };
    const Caso casos[] = {
        {"regular 16 ms        ", {16}},
        {"irregular 7/31/16/9  ", {7, 31, 16, 9}},
        {"com pausa de 400 ms  ", {16, 16, 16, 400}},
    };
    for (const Caso& c : casos) {
        Resultado r = simular(c.eventos);
        std::cout << c.nome << " explode na atualizacao " << r.atualizacoes
                  << " (simulado=" << r.atualizacoes * 8 << " ms, real=" << r.tempoRealMs << " ms)\n";
    }
    return 0;
}
