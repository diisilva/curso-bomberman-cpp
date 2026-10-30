// Aula 14 — Chefes, projéteis e Mario: os números por trás da luta
//
// Três regras do jogo real (projeto_final/bomberman.cpp):
//   vidaMaximaBoss       → a vida de cada um dos cinco chefes
//   atingirBoss          → 4 de dano por bomba, uma vez por bomba, com 700 ms de imunidade
//   atualizarProjeteisBoss → o projétil anda em subpassos de no máximo 25 ms
#include <algorithm>
#include <cmath>
#include <cstdio>

const int VIDA_BOSS = 100;
const float FATOR_FORCA_BOSS = 1.15f;
const int IMUNIDADE_BOSS_MS = 700;

// Fases pares 2, 4, 6, 8, 10 → índice 0..4 → Golem, Egg-HVC, Hornet, Guardião, Bowser.
int vidaMaximaBoss(int indice) {
    const int vidas[] = {48, 64, 80, VIDA_BOSS, 112};
    return int(std::round(vidas[indice] * FATOR_FORCA_BOSS));
}

// ---------- dano no chefe ----------
struct Boss { int vida = 0, imunidadeMs = 0; };
struct Bomba { bool bossAtingido = false; };

void atingirBoss(Boss& boss, Bomba& b) {
    if (boss.imunidadeMs > 0 || b.bossAtingido) return;   // as duas travas do jogo
    b.bossAtingido = true;                               // a chama dura 650 ms: sem isso, 81 acertos
    boss.vida = std::max(0, boss.vida - 4);
    boss.imunidadeMs = IMUNIDADE_BOSS_MS;
}

// ---------- projétil ----------
// Anda "dt" segundos. Com subpassos, testa o toque a cada 25 ms no máximo;
// sem eles, testa só no fim do quadro e pode pular por cima do alvo.
float moverProjetil(float c, float vc, float dt, bool subpassos, float alvoC, bool& acertou) {
    float restante = dt;
    while (restante > 0) {
        float passo = subpassos ? std::min(restante, .025f) : restante;
        restante -= passo;
        c += vc * passo;
        if (std::fabs(c - alvoC - .5f) < .48f) { acertou = true; return c; }   // mesmo raio do jogo
    }
    return c;
}

int main() {
    const char* nomes[] = {"Golem", "Egg-HVC", "Hornet", "Guardiao", "Bowser"};
    std::printf("vida dos chefes:");
    for (int i = 0; i < 5; ++i) std::printf(" %s=%d", nomes[i], vidaMaximaBoss(i));
    std::printf("\n");

    Boss golem; golem.vida = vidaMaximaBoss(0);
    Bomba b1, b2;
    atingirBoss(golem, b1);  std::printf("bomba 1 acerta:              vida=%d\n", golem.vida);
    atingirBoss(golem, b1);  std::printf("a mesma chama no passo seguinte: vida=%d (bossAtingido)\n", golem.vida);
    golem.imunidadeMs -= 300;
    atingirBoss(golem, b2);  std::printf("bomba 2, 300 ms depois:       vida=%d (imune por mais %d ms)\n", golem.vida, golem.imunidadeMs);
    golem.imunidadeMs = 0; b2.bossAtingido = false;
    atingirBoss(golem, b2);  std::printf("bomba 2, depois da imunidade: vida=%d\n", golem.vida);

    // Projétil a 12 células/s, jogador na casa 3; um quadro atrasado de 250 ms.
    for (bool sub : {false, true}) {
        bool acertou = false;
        float c = moverProjetil(1.0f, 12.0f, 0.25f, sub, 3.0f, acertou);
        std::printf("%s: ", sub ? "com subpassos de 25 ms" : "sem subpassos         ");
        if (acertou) std::printf("acertou em c=%.2f\n", c);
        else std::printf("passou por cima do jogador (c=%.2f)\n", c);
    }
    return 0;
}
