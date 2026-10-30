// Aula 29 — solução dos exercícios 2, 3 e 4.
//
// 2) Um template que recebe um critério (lambda), como bfsProximoPasso.
// 3) Captura por valor [=] e por referência [&]: o jogo usa [&] quando a
//    lambda precisa ler variáveis que mudam (ex.: o alvo na BFS).
// 4) unique_ptr dentro de um vetor: não pode ser copiado, só movido.
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

template <typename T, typename Criterio>
int encontrarPrimeiro(const std::vector<T>& itens, Criterio criterio) {
    for (int i = 0; i < int(itens.size()); ++i)
        if (criterio(itens[i])) return i;
    return -1;   // como bfsProximoPasso devolvendo a origem: "não achei"
}

struct Fonte { int tamanho; };

int main() {
    std::vector<int> vidas = {55, 74, 92, 115, 129};
    int minimo = 90;
    std::printf("primeiro chefe com vida >= %d: indice %d\n", minimo,
                encontrarPrimeiro(vidas, [&](int v) { return v >= minimo; }));

    auto porValor = [=](int v) { return v >= minimo; };       // copiou minimo = 90 agora
    auto porReferencia = [&](int v) { return v >= minimo; };  // vai ler minimo quando for chamada
    minimo = 120;
    std::printf("depois de minimo=120: [=] acha %d, [&] acha %d\n",
                encontrarPrimeiro(vidas, porValor), encontrarPrimeiro(vidas, porReferencia));

    // Como cacheFontes: cada tamanho de fonte é criado uma vez e guardado.
    std::vector<std::unique_ptr<Fonte>> cache;
    auto nova = std::make_unique<Fonte>(Fonte{18});
    cache.push_back(std::move(nova));        // push_back(nova) não compila: unique_ptr não copia
    std::printf("cache: %zu fonte(s); 'nova' ficou vazia? %s\n", cache.size(), nova ? "nao" : "sim");
    return 0;
}
