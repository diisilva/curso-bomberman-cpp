// Aula 01 — solução do exercício 2: uma função que DEVOLVE a nova posição.
//
// moverJogador, no jogo, não altera "jogador" direto: primeiro calcula
//     Posicao destino{jogador.linha + dl, jogador.coluna + dc};
// e só depois de testar a colisão faz jogador = destino. A função "somar"
// abaixo reproduz esse estilo: recebe por valor e devolve um valor novo.
#include <iostream>

struct Posicao { int linha, coluna; };

bool iguais(Posicao a, Posicao b) {
    return a.linha == b.linha && a.coluna == b.coluna;
}

// Retorno por valor: a origem continua intacta; quem chama decide se aceita.
Posicao somar(Posicao origem, int dl, int dc) {
    return Posicao{origem.linha + dl, origem.coluna + dc};
}

int main() {
    const char* mapa[] = {"#####", "#..##", "#####"};  // (1,3) é parede
    Posicao jogador{1, 1};

    for (int tentativa = 0; tentativa < 3; ++tentativa) {
        Posicao destino = somar(jogador, 0, 1);
        bool livre = mapa[destino.linha][destino.coluna] == '.';
        if (livre) jogador = destino;  // aceita só depois de testar
        std::cout << "destino=(" << destino.linha << "," << destino.coluna << ") livre=" << livre
                  << " jogador=(" << jogador.linha << "," << jogador.coluna << ")\n";
    }
    std::cout << "parou em (1,2)? " << iguais(jogador, Posicao{1, 2}) << "\n";
    return 0;
}
