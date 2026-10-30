// Aula 20 — BFS de fuga segura: o bot só planta onde consegue fugir
//
// Funções do jogo real (projeto_final/bomberman.cpp):
//   alvoParaBomba        → há caixa (ou inimigo) no alcance desta casa?
//   celulaNoAlcance      → a previsão da chama (Aula 11)
//   existeFugaSegura     → BFS: há casa fora da chama a até 7 passos?
//   alvoParaBombaSeguro  → os dois juntos: só planta se der para fugir
// Aqui a BFS devolve QUANTOS passos (o jogo só precisa de sim/não).
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <string>
#include <utility>
#include <vector>

const int TEMPO_BOMBA_MS = 2200;
int INTERVALO_MOVIMENTO_MS = 280;   // const no jogo; aqui variável para testar o prazo
int alcanceExplosao = 1;
std::vector<std::string> mapa;       // '#' parede, '+' caixa, '.' chão

struct Posicao { int linha, coluna; };
bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool dentro(int l, int c) { return l >= 0 && l < int(mapa.size()) && c >= 0 && c < int(mapa[0].size()); }
bool podeAndarLivre(Posicao p) { return dentro(p.linha, p.coluna) && mapa[p.linha][p.coluna] == '.'; }

bool alvoParaBomba(Posicao p) {
    const int direcoes[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int d = 0; d < 4; d++)
        for (int distancia = 1; distancia <= alcanceExplosao; distancia++) {
            int l = p.linha + direcoes[d][0] * distancia, c = p.coluna + direcoes[d][1] * distancia;
            if (!dentro(l, c) || mapa[l][c] == '#') break;
            if (mapa[l][c] == '+') return true;
        }
    return false;
}

bool celulaNoAlcance(Posicao origemBomba, Posicao p, int alcance) {
    if (iguais(p, origemBomba)) return true;
    int dl = p.linha - origemBomba.linha, dc = p.coluna - origemBomba.coluna;
    if (dl != 0 && dc != 0) return false;
    int distancia = std::abs(dl) + std::abs(dc);
    if (distancia > alcance) return false;
    int passoL = (dl > 0) - (dl < 0), passoC = (dc > 0) - (dc < 0);
    for (int i = 1; i <= distancia; i++) {
        char celula = mapa[origemBomba.linha + passoL * i][origemBomba.coluna + passoC * i];
        if (celula == '#') return false;
        if (celula == '+') return i == distancia;
    }
    return true;
}

// Mesmo laço de existeFugaSegura; devolve os passos até a casa segura, ou -1.
int passosAteSeguro(Posicao origem) {
    int passosDisponiveis = TEMPO_BOMBA_MS / INTERVALO_MOVIMENTO_MS;
    const int direcoes[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    std::vector<std::vector<bool>> visitado(mapa.size(), std::vector<bool>(mapa[0].size()));
    std::queue<std::pair<Posicao, int>> fila;
    fila.push({origem, 0});
    visitado[origem.linha][origem.coluna] = true;
    while (!fila.empty()) {
        auto [atual, passos] = fila.front();
        fila.pop();
        if (!iguais(atual, origem) && !celulaNoAlcance(origem, atual, alcanceExplosao)) return passos;
        if (passos >= passosDisponiveis) continue;   // o prazo acabou neste ramo
        for (const auto& d : direcoes) {
            Posicao proxima{atual.linha + d[0], atual.coluna + d[1]};
            if (!podeAndarLivre(proxima) || visitado[proxima.linha][proxima.coluna]) continue;
            visitado[proxima.linha][proxima.coluna] = true;
            fila.push({proxima, passos + 1});
        }
    }
    return -1;
}
bool existeFugaSegura(Posicao origem) { return passosAteSeguro(origem) >= 0; }
bool alvoParaBombaSeguro(Posicao p) { return alvoParaBomba(p) && existeFugaSegura(p); }

void testar(const char* nome, Posicao bot) {
    int passos = passosAteSeguro(bot);
    std::printf("%-34s prazo=%d passos  alvo=%d  fuga=%-11s -> %s\n", nome, TEMPO_BOMBA_MS / INTERVALO_MOVIMENTO_MS,
                alvoParaBomba(bot), passos < 0 ? "nao" : ("em " + std::to_string(passos) + " passos").c_str(),
                alvoParaBombaSeguro(bot) ? "PLANTA" : "nao planta");
}

int main() {
    mapa = {"#######",
            "#+...##",      // bot em (1,2), ao lado de uma caixa; corredor sem saída
            "#######"};
    alcanceExplosao = 1; testar("corredor curto, alcance 1", {1, 2});
    alcanceExplosao = 2; testar("corredor curto, alcance 2", {1, 2});

    mapa = {"###########",
            "#+........#",  // corredor longo que dobra para baixo no fim
            "#########.#",
            "#########.#",
            "###########"};
    alcanceExplosao = 5;
    INTERVALO_MOVIMENTO_MS = 280; testar("corredor longo, alcance 5", {1, 2});
    INTERVALO_MOVIMENTO_MS = 440; testar("corredor longo, passo de 440 ms", {1, 2});
    return 0;
}
