// Aula 22 — solução do exercício 2: a guarda e o contra-ataque da Hornet.
//
// No jogo: H_GUARDA dura 1200 ms. Quando acaba, se o jogador estiver a menos
// de 3 casas, a Hornet contra-ataca (H_CONTRA, 630 ms, solta 4 fios); senão,
// só se recupera (H_RECUP_CONTRA, 480 ms). Se ela levar dano DURANTE a guarda,
// guarda um "contra pendente" e contra-ataca logo depois de se recuperar.
#include <algorithm>
#include <cstdio>

enum EtapaHornet { H_IDLE, H_GUARDA, H_CONTRA, H_RECUP_CONTRA, H_DANO, H_RECUP_DANO };
const char* nome[] = {"H_IDLE", "H_GUARDA", "H_CONTRA", "H_RECUP_CONTRA", "H_DANO", "H_RECUP_DANO"};

struct Boss { EtapaHornet etapaHornet = H_IDLE; int tempoEtapaHornetMs = 0; bool contraPendenteHornet = false; } boss;
int relogio = 0;
float distanciaJogador = 0;

void entrarEtapaHornet(EtapaHornet etapa, int duracao) {
    std::printf("  t=%4d %-14s -> %s\n", relogio, nome[boss.etapaHornet], nome[etapa]);
    boss.etapaHornet = etapa;
    boss.tempoEtapaHornetMs = duracao;
}

void reagirDanoHornet() {
    boss.contraPendenteHornet = boss.contraPendenteHornet || boss.etapaHornet == H_GUARDA;
    entrarEtapaHornet(H_DANO, 480);
}

void atualizarHornet(int passo) {
    if (boss.etapaHornet == H_IDLE) return;
    boss.tempoEtapaHornetMs = std::max(0, boss.tempoEtapaHornetMs - passo);
    if (boss.tempoEtapaHornetMs > 0) return;
    switch (boss.etapaHornet) {
        case H_GUARDA:
            if (distanciaJogador < 3.f) entrarEtapaHornet(H_CONTRA, 630);   // + soltarFiosHornet(4)
            else entrarEtapaHornet(H_RECUP_CONTRA, 480);
            break;
        case H_CONTRA: entrarEtapaHornet(H_RECUP_CONTRA, 480); break;
        case H_DANO:   entrarEtapaHornet(H_RECUP_DANO, 420); break;
        case H_RECUP_DANO:
            if (boss.contraPendenteHornet) { boss.contraPendenteHornet = false; entrarEtapaHornet(H_CONTRA, 630); break; }
            [[fallthrough]];
        default: entrarEtapaHornet(H_IDLE, 0); break;
    }
}

void cenario(const char* titulo, float distancia, int danoEm) {
    std::printf("%s\n", titulo);
    boss = Boss{};
    distanciaJogador = distancia;
    relogio = 0;
    entrarEtapaHornet(H_GUARDA, 1200);
    for (relogio = 16; relogio <= 3000 && boss.etapaHornet != H_IDLE; relogio += 16) {
        if (relogio == danoEm) reagirDanoHornet();
        atualizarHornet(16);
    }
}

int main() {
    cenario("jogador perto (2 casas):", 2.f, -1);
    cenario("jogador longe (5 casas):", 5.f, -1);
    cenario("bomba acerta durante a guarda:", 5.f, 400);
    return 0;
}
