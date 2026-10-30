// Aula 09 — Os oito poderes
//
// Como o jogo real (projeto_final/bomberman.cpp) esconde, revela, coleta e
// zera os itens. Os nomes das variáveis e das funções são os do jogo:
//   posicionarBlocosBonus → o item fica escondido num PAREDE_BONUS
//   destruirBlocoBonus    → a chama quebra o bloco e o item cai no chão
//   coletarItensEm        → o jogador pisa e o efeito é aplicado
//   montarFase            → os bônus voltam ao valor inicial
#include <iostream>
#include <vector>

struct Posicao { int linha, coluna; };
enum TipoItem { CHUTE, TELEPORTE, RAIO, BOMBA_EXTRA, VIDA_EXTRA, BOMBA_RELOGIO, IMUNE_BOMBA, ATRAVESSA_BLOCO };
struct Item { Posicao posicao; TipoItem tipo; };
struct BlocoBonus { Posicao posicao; TipoItem tipo; };

const int ALCANCE_EXPLOSAO_INICIAL = 1, ALCANCE_EXPLOSAO_MAXIMO = 5, PONTOS_POR_ITEM = 30;

// No jogo, cada efeito é uma variável global separada.
bool possuiCartaChute = false, possuiCartaTeleporte = false;
int alcanceExplosao = ALCANCE_EXPLOSAO_INICIAL;
int bombasExtras = 0;
int vidas = 3;
bool temBombaRelogio = false, imuneAPropriaBomba = false, atravessaBlocoQuebravel = false;
int pontos = 0;

std::vector<BlocoBonus> blocosBonus;   // itens ainda escondidos
std::vector<Item> itensNoChao;         // itens revelados, esperando alguém pisar

bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
int limiteBombas() { return 1 + bombasExtras; }

void destruirBlocoBonus(int linha, int coluna) {
    for (size_t i = 0; i < blocosBonus.size(); i++) {
        if (blocosBonus[i].posicao.linha == linha && blocosBonus[i].posicao.coluna == coluna) {
            itensNoChao.push_back({blocosBonus[i].posicao, blocosBonus[i].tipo});
            blocosBonus.erase(blocosBonus.begin() + i);
            return;
        }
    }
}

void coletarItensEm(Posicao pos) {
    for (size_t i = 0; i < itensNoChao.size(); i++) {
        if (!iguais(itensNoChao[i].posicao, pos)) continue;
        switch (itensNoChao[i].tipo) {
            case CHUTE: possuiCartaChute = true; break;
            case TELEPORTE: possuiCartaTeleporte = true; break;
            case RAIO: if (alcanceExplosao < ALCANCE_EXPLOSAO_MAXIMO) alcanceExplosao++; break;
            case BOMBA_EXTRA: bombasExtras++; break;
            case VIDA_EXTRA: vidas++; break;
            case BOMBA_RELOGIO: temBombaRelogio = true; break;
            case IMUNE_BOMBA: imuneAPropriaBomba = true; break;
            case ATRAVESSA_BLOCO: atravessaBlocoQuebravel = true; break;
        }
        itensNoChao.erase(itensNoChao.begin() + i);
        pontos += PONTOS_POR_ITEM;
        return;   // um item por casa: pode sair logo
    }
}

// O trecho de montarFase que zera os poderes. "vidas" fica de fora de propósito.
void zerarPoderesDaFase() {
    itensNoChao.clear();
    possuiCartaChute = false;
    possuiCartaTeleporte = false;
    alcanceExplosao = ALCANCE_EXPLOSAO_INICIAL;
    bombasExtras = 0;
    temBombaRelogio = false;
    imuneAPropriaBomba = false;
    atravessaBlocoQuebravel = false;
}

int main() {
    blocosBonus = {{{1, 2}, RAIO}, {{1, 3}, BOMBA_EXTRA}, {{3, 3}, VIDA_EXTRA}};

    coletarItensEm({1, 2});
    std::cout << "pisou em (1,2) antes da explosao: alcance=" << alcanceExplosao << " (nada no chao)\n";

    destruirBlocoBonus(1, 2);
    destruirBlocoBonus(1, 3);
    std::cout << "chama quebrou 2 blocos: escondidos=" << blocosBonus.size() << " no chao=" << itensNoChao.size() << "\n";

    coletarItensEm({1, 2});
    coletarItensEm({1, 3});
    std::cout << "coletou: alcance=" << alcanceExplosao << " limite=" << limiteBombas() << " pontos=" << pontos << "\n";

    for (int i = 0; i < 6; ++i) {   // seis cartas RAIO seguidas
        itensNoChao.push_back({{2, 2}, RAIO});
        coletarItensEm({2, 2});
    }
    itensNoChao.push_back({{2, 2}, VIDA_EXTRA});
    coletarItensEm({2, 2});
    std::cout << "mais 6 RAIO e 1 VIDA: alcance=" << alcanceExplosao << " (teto " << ALCANCE_EXPLOSAO_MAXIMO
              << ") vidas=" << vidas << "\n";

    zerarPoderesDaFase();
    std::cout << "nova fase: alcance=" << alcanceExplosao << " limite=" << limiteBombas() << " vidas=" << vidas << "\n";
    return 0;
}
