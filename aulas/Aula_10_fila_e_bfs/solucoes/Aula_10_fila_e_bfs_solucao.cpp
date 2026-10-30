// Aula 10 — solução dos exercícios 2 e 3.
//
// 2) O jogo só precisa do PRÓXIMO passo, porque recalcula a cada decisão.
//    Para desenhar a rota (ou depurar o bot), seguimos "anterior" até a
//    origem e invertemos a lista.
// 3) A mesma busca, com outro ehAlvo: parar na primeira casa vizinha de uma
//    caixa. É assim que o bot do jogo escolhe onde plantar (alvoParaBombaSeguro).
#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const int LINHAS = 5, COLUNAS = 7;
const char* mapa[LINHAS] = {"#######",
                            "#..#..#",
                            "#..#.+#",     // uma caixa em (2,5)
                            "#.....#",
                            "#######"};

struct Posicao { int linha, coluna; };
bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool podeAndarLivre(Posicao p) { return mapa[p.linha][p.coluna] == '.'; }

template <typename EhAlvo, typename PodeAndarPor>
std::vector<Posicao> rotaCompleta(Posicao origem, EhAlvo ehAlvo, PodeAndarPor podeAndarPor) {
    const int direcoes[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool visitado[LINHAS][COLUNAS]{};
    Posicao anterior[LINHAS][COLUNAS];
    std::queue<Posicao> fila;
    fila.push(origem);
    visitado[origem.linha][origem.coluna] = true;
    while (!fila.empty()) {
        Posicao atual = fila.front();
        fila.pop();
        if (ehAlvo(atual)) {
            std::vector<Posicao> rota{atual};
            while (!iguais(rota.back(), origem)) rota.push_back(anterior[rota.back().linha][rota.back().coluna]);
            std::reverse(rota.begin(), rota.end());   // da origem até o alvo
            return rota;
        }
        for (const auto& d : direcoes) {
            Posicao p{atual.linha + d[0], atual.coluna + d[1]};
            if (!podeAndarPor(p) || visitado[p.linha][p.coluna]) continue;
            visitado[p.linha][p.coluna] = true;
            anterior[p.linha][p.coluna] = atual;
            fila.push(p);
        }
    }
    return {};   // sem caminho
}

void imprimir(const char* nome, const std::vector<Posicao>& rota) {
    std::cout << nome << " (" << rota.size() - 1 << " passos): ";
    for (const Posicao& p : rota) std::cout << "(" << p.linha << "," << p.coluna << ") ";
    std::cout << "\n";
}

int main() {
    Posicao inicio{1, 1}, alvo{1, 5};
    imprimir("ate o jogador", rotaCompleta(inicio, [&](Posicao p) { return iguais(p, alvo); }, podeAndarLivre));

    auto vizinhaDeCaixa = [](Posicao p) {
        const int dl[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
        for (int d = 0; d < 4; ++d)
            if (mapa[p.linha + dl[d]][p.coluna + dc[d]] == '+') return true;
        return false;
    };
    imprimir("ate uma caixa", rotaCompleta(inicio, vizinhaDeCaixa, podeAndarLivre));
    return 0;
}
