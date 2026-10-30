// Aula 10 — Fila e busca em largura (BFS)
//
// bfsProximoPasso é copiada do jogo real (projeto_final/bomberman.cpp). É a
// MESMA função que faz o inimigo perseguir o jogador, o chefe andar e o bot
// procurar caixa e fuga: muda só o que se passa em ehAlvo e podeAndarPor.
// mapaDeDistancias é uma BFS extra, só para imprimir as "camadas" da busca.
#include <iostream>
#include <queue>

const int LINHAS = 5, COLUNAS = 7;
const char* mapa[LINHAS] = {"#######",
                            "#..#..#",
                            "#..#..#",
                            "#.....#",
                            "#######"};

struct Posicao { int linha, coluna; };
bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool podeAndarLivre(Posicao p) { return mapa[p.linha][p.coluna] == '.'; }

// Template porque cada lambda tem um tipo próprio (ver Aula 29).
template <typename EhAlvo, typename PodeAndarPor>
Posicao bfsProximoPasso(Posicao origem, EhAlvo ehAlvo, PodeAndarPor podeAndarPor) {
    const int direcoes[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool visitado[LINHAS][COLUNAS]{};
    Posicao anterior[LINHAS][COLUNAS];     // de onde viemos para chegar a cada casa
    std::queue<Posicao> fila;
    fila.push(origem);
    visitado[origem.linha][origem.coluna] = true;
    Posicao objetivo = origem;
    bool encontrou = false;

    while (!fila.empty() && !encontrou) {
        Posicao atual = fila.front();      // FIFO: sai primeiro quem entrou primeiro
        fila.pop();
        if (ehAlvo(atual)) { objetivo = atual; encontrou = true; break; }
        for (const auto& direcao : direcoes) {
            Posicao proxima{atual.linha + direcao[0], atual.coluna + direcao[1]};
            if (!podeAndarPor(proxima) || visitado[proxima.linha][proxima.coluna]) continue;
            visitado[proxima.linha][proxima.coluna] = true;   // marcar ao ENTRAR na fila
            anterior[proxima.linha][proxima.coluna] = atual;
            fila.push(proxima);
        }
    }

    if (!encontrou || iguais(objetivo, origem)) return origem;
    // Volta pelo rastro até a casa vizinha da origem: esse é o próximo passo.
    while (!iguais(anterior[objetivo.linha][objetivo.coluna], origem))
        objetivo = anterior[objetivo.linha][objetivo.coluna];
    return objetivo;
}

// Igual ao jogo: o inimigo persegue chamando a BFS com "parar no alvo".
Posicao proximoPassoParaAlvo(Posicao origem, Posicao alvo) {
    return bfsProximoPasso(origem, [&](Posicao p) { return iguais(p, alvo); }, podeAndarLivre);
}

void mapaDeDistancias(Posicao origem) {
    int distancia[LINHAS][COLUNAS];
    for (auto& linha : distancia) for (int& d : linha) d = -1;
    std::queue<Posicao> fila;
    fila.push(origem);
    distancia[origem.linha][origem.coluna] = 0;
    while (!fila.empty()) {
        Posicao atual = fila.front(); fila.pop();
        const int dl[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
        for (int d = 0; d < 4; ++d) {
            Posicao p{atual.linha + dl[d], atual.coluna + dc[d]};
            if (!podeAndarLivre(p) || distancia[p.linha][p.coluna] >= 0) continue;
            distancia[p.linha][p.coluna] = distancia[atual.linha][atual.coluna] + 1;
            fila.push(p);
        }
    }
    for (int l = 0; l < LINHAS; ++l) {
        for (int c = 0; c < COLUNAS; ++c)
            std::cout << (mapa[l][c] == '#' ? '#' : char('0' + distancia[l][c])) << ' ';
        std::cout << "\n";
    }
}

int main() {
    Posicao inimigo{1, 1}, jogador{1, 5};
    std::cout << "distancias a partir do inimigo em (1,1):\n";
    mapaDeDistancias(inimigo);
    Posicao passo = proximoPassoParaAlvo(inimigo, jogador);
    std::cout << "jogador em (1,5); primeiro passo do inimigo=(" << passo.linha << "," << passo.coluna << ")\n";
    Posicao parado = proximoPassoParaAlvo(jogador, jogador);
    std::cout << "ja no alvo: devolve a propria origem=(" << parado.linha << "," << parado.coluna << ")\n";
    return 0;
}
