// Aula 14 — solução dos exercícios 2 e 3.
//
// 2) Quantas bombas derrubam cada chefe? Cada acerto tira 4: são
//    ceil(vida / 4) acertos. A imunidade de 700 ms limita o ritmo: no máximo
//    um acerto a cada 0,7 s, mesmo com várias bombas explodindo juntas.
// 3) Até que velocidade o subpasso de 25 ms é seguro? O alvo tem uma janela
//    de toque de 0,96 célula (raio 0,48). Se o projétil anda 0,96 ou mais
//    num subpasso, pode pular a janela inteira.
#include <cmath>
#include <cstdio>

int vidaMaximaBoss(int indice) {
    const int vidas[] = {48, 64, 80, 100, 112};
    return int(std::round(vidas[indice] * 1.15f));
}

int main() {
    const char* nomes[] = {"Golem", "Egg-HVC", "Hornet", "Guardiao", "Bowser"};
    for (int i = 0; i < 5; ++i) {
        int vida = vidaMaximaBoss(i);
        int acertos = (vida + 3) / 4;   // ceil(vida / 4) só com inteiros
        std::printf("%-9s vida=%3d -> %2d acertos, no minimo %.1f s de luta\n",
                    nomes[i], vida, acertos, (acertos - 1) * 0.7);
    }
    const float janela = 2 * 0.48f, subpasso = 0.025f;
    std::printf("subpasso de 25 ms e seguro ate %.1f celulas/s (os projeteis do jogo andam de 2,1 a 3,6)\n",
                janela / subpasso);
    return 0;
}
