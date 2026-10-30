// Aula 20 — solução dos exercícios 2 e 3: para onde fugir, com uma ou duas bombas.
//
// Depois de plantar, o bot chama proximoPassoParaFuga: uma BFS (a mesma
// bfsProximoPasso da Aula 10) que para na primeira casa onde perigoDaBomba é
// falso. perigoDaBomba olha TODAS as bombas ativas, então uma segunda bomba
// pode fechar a única saída.
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <string>
#include <vector>

std::vector<std::string> mapa = {"###########",
                                 "#+........#",
                                 "#########.#",
                                 "#########.#",
                                 "###########"};
struct Posicao { int linha, coluna; };
struct Bomba { Posicao posicao; int alcance; };
std::vector<Bomba> bombas;

bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool livre(Posicao p) { return mapa[p.linha][p.coluna] == '.'; }

bool celulaNoAlcance(Posicao o, Posicao p, int alcance) {
    if (iguais(p, o)) return true;
    int dl = p.linha - o.linha, dc = p.coluna - o.coluna;
    if ((dl != 0 && dc != 0) || std::abs(dl) + std::abs(dc) > alcance) return false;
    int pl = (dl > 0) - (dl < 0), pc = (dc > 0) - (dc < 0);
    for (int i = 1; i <= std::abs(dl) + std::abs(dc); i++) {
        char celula = mapa[o.linha + pl * i][o.coluna + pc * i];
        if (celula == '#') return false;
        if (celula == '+') return i == std::abs(dl) + std::abs(dc);
    }
    return true;
}
bool perigoDaBomba(Posicao p) {
    for (const Bomba& b : bombas) if (celulaNoAlcance(b.posicao, p, b.alcance)) return true;
    return false;
}

// bfsProximoPasso da Aula 10, devolvendo a rota inteira para podermos imprimir.
std::vector<Posicao> rotaDeFuga(Posicao origem) {
    std::vector<std::vector<int>> anterior(mapa.size(), std::vector<int>(mapa[0].size(), -1));
    std::queue<Posicao> fila;
    fila.push(origem);
    anterior[origem.linha][origem.coluna] = origem.linha * 100 + origem.coluna;
    const int dl[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
    while (!fila.empty()) {
        Posicao atual = fila.front(); fila.pop();
        if (!perigoDaBomba(atual)) {
            std::vector<Posicao> rota{atual};
            while (!iguais(rota.back(), origem)) {
                int a = anterior[rota.back().linha][rota.back().coluna];
                rota.push_back({a / 100, a % 100});
            }
            return {rota.rbegin(), rota.rend()};
        }
        for (int d = 0; d < 4; d++) {
            Posicao p{atual.linha + dl[d], atual.coluna + dc[d]};
            if (!livre(p) || anterior[p.linha][p.coluna] >= 0) continue;
            anterior[p.linha][p.coluna] = atual.linha * 100 + atual.coluna;
            fila.push(p);
        }
    }
    return {};
}

void mostrar(const char* titulo, Posicao bot) {
    std::vector<Posicao> rota = rotaDeFuga(bot);
    std::printf("%s ", titulo);
    if (rota.empty()) { std::printf("sem casa segura alcancavel\n"); return; }
    std::printf("primeiro passo=(%d,%d), %zu passos:", rota[1].linha, rota[1].coluna, rota.size() - 1);
    for (const Posicao& p : rota) std::printf(" (%d,%d)", p.linha, p.coluna);
    std::printf("\n");
}

int main() {
    Posicao bot{1, 2};
    bombas = {{bot, 5}};
    mostrar("uma bomba:  ", bot);
    bombas.push_back({{1, 9}, 3});   // um inimigo plantou na curva do corredor
    mostrar("duas bombas:", bot);
    return 0;
}
