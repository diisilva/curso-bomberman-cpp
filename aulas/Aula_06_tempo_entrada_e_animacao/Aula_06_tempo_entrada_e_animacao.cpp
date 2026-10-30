// Aula 06 — Tempo, entrada e animação
//
// Três peças do laço do jogo (projeto_final/bomberman.cpp), sem a janela:
//   1. o acumulador de WM_TIMER, que roda atualizar(8) quantas vezes couber;
//   2. a escolha da direção pela tecla apertada por ÚLTIMO (ordemTecla);
//   3. quadroCaminhada, que escolhe a pose pela distância andada.
#include <algorithm>
#include <iostream>
#include <vector>

// ---------- 1. passo fixo ----------
int atualizacoes = 0;
void atualizar(int tempoMs) { (void)tempoMs; ++atualizacoes; }   // no jogo: bombas, inimigos, movimento...

// Mesmo miolo do case WM_TIMER do jogo. "decorrido" é o tempo REAL desde o
// último evento, medido com steady_clock; aqui ele vem pronto da lista.
int acumuladoMs = 0;
void aoReceberTimer(int decorrido) {
    acumuladoMs += std::min(decorrido, 100);   // teto: uma pausa longa não vira avalanche
    while (acumuladoMs >= 8) {
        atualizar(8);
        acumuladoMs -= 8;
    }
}

// ---------- 2. tecla mais recente ----------
bool teclaPressionada[256] = {};
unsigned long long ordemTecla[256] = {}, proximaOrdemTecla = 0;
void aoApertar(int tecla) {
    if (!teclaPressionada[tecla]) ordemTecla[tecla] = ++proximaOrdemTecla;   // só no primeiro aperto
    teclaPressionada[tecla] = true;
}
void aoSoltar(int tecla) { teclaPressionada[tecla] = false; }

struct Entrada { int tecla, dl, dc; };
char direcaoEscolhida() {
    const Entrada letras[] = {{'W', -1, 0}, {'S', 1, 0}, {'A', 0, -1}, {'D', 0, 1}};
    std::vector<Entrada> entradas;
    for (auto e : letras) if (teclaPressionada[e.tecla]) entradas.push_back(e);
    // Igual a processarMovimentoContinuo: a ordem maior (mais recente) vem primeiro.
    std::stable_sort(entradas.begin(), entradas.end(),
                     [](const Entrada& a, const Entrada& b) { return ordemTecla[a.tecla] > ordemTecla[b.tecla]; });
    return entradas.empty() ? '-' : static_cast<char>(entradas.front().tecla);
}

// ---------- 3. pose pela distância ----------
int quadroCaminhada(float distancia, bool andando) {
    const int ciclo[] = {0, 1, 0, 2};
    return andando ? ciclo[static_cast<int>(distancia * 2.5f) % 4] : 1;
}

int main() {
    std::cout << "-- 1. passo fixo de 8 ms --\n";
    for (int evento : {16, 40, 16, 250}) {
        int antes = atualizacoes;
        aoReceberTimer(evento);
        std::cout << "evento=" << evento << "ms -> atualizar(8) x" << atualizacoes - antes
                  << "  total=" << atualizacoes << " resto=" << acumuladoMs << "\n";
    }

    std::cout << "-- 2. tecla mais recente --\n";
    aoApertar('W');  std::cout << "segura W        -> anda " << direcaoEscolhida() << "\n";
    aoApertar('D');  std::cout << "segura W e D    -> anda " << direcaoEscolhida() << "\n";
    aoSoltar('D');   std::cout << "solta D         -> anda " << direcaoEscolhida() << "\n";

    std::cout << "-- 3. pose a cada 0,2 celula --\n";
    for (int i = 0; i <= 10; ++i) std::cout << quadroCaminhada(i * 0.2f, true) << ' ';
    std::cout << " (parado: " << quadroCaminhada(0, false) << ")\n";
    return 0;
}
