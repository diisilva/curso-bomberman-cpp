// Aula 09 — solução do exercício 3: os poderes numa struct só.
//
// No jogo há oito variáveis globais, e montarFase zera uma por uma. Se alguém
// criar um nono poder e esquecer de zerá-lo ali, o bônus "vaza" para a fase
// seguinte, e o compilador não avisa. Com uma struct com valores iniciais,
// zerar é uma linha só: poderes = Poderes{}. Um poder novo ganha o reset de graça.
#include <iostream>
#include <vector>

struct Posicao { int linha, coluna; };
enum TipoItem { CHUTE, TELEPORTE, RAIO, BOMBA_EXTRA, VIDA_EXTRA, BOMBA_RELOGIO, IMUNE_BOMBA, ATRAVESSA_BLOCO };
struct Item { Posicao posicao; TipoItem tipo; };

const int ALCANCE_EXPLOSAO_INICIAL = 1, ALCANCE_EXPLOSAO_MAXIMO = 5;

// Tudo o que zera ao trocar de fase. As vidas NÃO entram: elas são da partida.
struct Poderes {
    bool chute = false, teleporte = false;
    int alcance = ALCANCE_EXPLOSAO_INICIAL;
    int bombasExtras = 0;
    bool relogio = false, imuneAPropria = false, atravessaCaixa = false;
    int limiteBombas() const { return 1 + bombasExtras; }
};

Poderes poderes;
int vidas = 3;
std::vector<Item> itensNoChao;

bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }

void coletarItensEm(Posicao pos) {
    for (size_t i = 0; i < itensNoChao.size(); i++) {
        if (!iguais(itensNoChao[i].posicao, pos)) continue;
        switch (itensNoChao[i].tipo) {
            case CHUTE: poderes.chute = true; break;
            case TELEPORTE: poderes.teleporte = true; break;
            case RAIO: if (poderes.alcance < ALCANCE_EXPLOSAO_MAXIMO) poderes.alcance++; break;
            case BOMBA_EXTRA: poderes.bombasExtras++; break;
            case VIDA_EXTRA: vidas++; break;                       // fora da struct
            case BOMBA_RELOGIO: poderes.relogio = true; break;
            case IMUNE_BOMBA: poderes.imuneAPropria = true; break;
            case ATRAVESSA_BLOCO: poderes.atravessaCaixa = true; break;
        }
        itensNoChao.erase(itensNoChao.begin() + i);
        return;
    }
}

void zerarPoderesDaFase() {
    itensNoChao.clear();
    poderes = Poderes{};   // todos os campos voltam ao valor inicial declarado acima
}

int main() {
    for (int tipo = CHUTE; tipo <= ATRAVESSA_BLOCO; ++tipo) {
        itensNoChao.push_back({{1, 1}, static_cast<TipoItem>(tipo)});
        coletarItensEm({1, 1});
    }
    std::cout << "oito cartas: alcance=" << poderes.alcance << " limite=" << poderes.limiteBombas()
              << " chute=" << poderes.chute << " atravessa=" << poderes.atravessaCaixa << " vidas=" << vidas << "\n";
    zerarPoderesDaFase();
    std::cout << "nova fase:   alcance=" << poderes.alcance << " limite=" << poderes.limiteBombas()
              << " chute=" << poderes.chute << " atravessa=" << poderes.atravessaCaixa << " vidas=" << vidas << "\n";
    return 0;
}
