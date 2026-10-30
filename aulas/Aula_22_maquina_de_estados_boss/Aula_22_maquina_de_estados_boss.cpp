// Aula 22 — Máquina de estados do chefe (Hornet)
//
// No jogo real (projeto_final/bomberman.cpp), a Hornet tem 22 etapas no
// enum EtapaHornet. Cada etapa dura um tempo; quando o tempo acaba, um switch
// escolhe a próxima. Aqui está o ataque da agulha, com as durações reais:
//   H_IDLE → H_PREP_AGULHA (750) → H_LANCAR (360) → H_VOO_AGULHA (até a
//   agulha voltar, no máximo 3000) → H_RECOLHER (480) → H_IDLE
// e a reação a dano: H_DANO (480) → H_RECUP_DANO (420) → volta ao voo se a
// agulha ainda estiver no ar; senão, H_IDLE.
#include <algorithm>
#include <cstdio>

enum EtapaHornet { H_IDLE, H_PREP_AGULHA, H_LANCAR, H_VOO_AGULHA, H_RECOLHER, H_DANO, H_RECUP_DANO };
const char* nome[] = {"H_IDLE", "H_PREP_AGULHA", "H_LANCAR", "H_VOO_AGULHA", "H_RECOLHER", "H_DANO", "H_RECUP_DANO"};

struct Boss {
    EtapaHornet etapaHornet = H_IDLE;
    int tempoEtapaHornetMs = 0;
    int tempoProximoAtaqueMs = 0;
} boss;
int agulhaNoArMs = 0;   // no jogo: um ProjetilBoss AGULHA que vai e volta
int relogio = 0;

void entrarEtapaHornet(EtapaHornet etapa, int duracao) {
    std::printf("t=%4d  %-13s -> %s\n", relogio, nome[boss.etapaHornet], nome[etapa]);
    boss.etapaHornet = etapa;
    boss.tempoEtapaHornetMs = duracao;
}

void reagirDanoHornet() {   // chamada por atingirBoss (Aula 14)
    entrarEtapaHornet(H_DANO, 480);
}

void atualizarHornet(int passo) {
    agulhaNoArMs = std::max(0, agulhaNoArMs - passo);
    boss.tempoProximoAtaqueMs -= passo;
    if (boss.etapaHornet == H_IDLE) {
        if (boss.tempoProximoAtaqueMs <= 0) entrarEtapaHornet(H_PREP_AGULHA, 750);   // criarPoderBoss
        return;
    }
    boss.tempoEtapaHornetMs = std::max(0, boss.tempoEtapaHornetMs - passo);
    // Durante o voo, a etapa termina quando a agulha volta, não pelo relógio.
    if (boss.etapaHornet == H_VOO_AGULHA && agulhaNoArMs == 0) boss.tempoEtapaHornetMs = 0;
    if (boss.tempoEtapaHornetMs > 0) return;
    switch (boss.etapaHornet) {
        case H_PREP_AGULHA: entrarEtapaHornet(H_LANCAR, 360); agulhaNoArMs = 2000; break;   // soltarAgulhaHornet (vai e volta em ~2 s)
        case H_LANCAR:      entrarEtapaHornet(H_VOO_AGULHA, 3000); break;
        case H_VOO_AGULHA:  entrarEtapaHornet(H_RECOLHER, 480); break;
        case H_DANO:        entrarEtapaHornet(H_RECUP_DANO, 420); break;
        case H_RECUP_DANO:
            if (agulhaNoArMs > 0) { entrarEtapaHornet(H_VOO_AGULHA, 3000); break; }
            [[fallthrough]];   // sem agulha no ar: recuperação comum
        default:
            entrarEtapaHornet(H_IDLE, 0);
            boss.tempoProximoAtaqueMs = 1000;
            break;
    }
}

int main() {
    for (relogio = 16; relogio <= 3600; relogio += 16) {   // o jogo atualiza a Hornet em passos de 16 ms
        if (relogio == 1296) {                             // uma bomba acerta durante o voo
            std::printf("t=%4d  ** bomba acerta a Hornet **\n", relogio);
            reagirDanoHornet();
        }
        atualizarHornet(16);
    }
    return 0;
}
