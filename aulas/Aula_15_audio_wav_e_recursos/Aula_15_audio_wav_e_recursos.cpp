// Aula 15 — Áudio: amostras PCM
//
// A música do jogo não vem de um arquivo: gerarMusicaProcedural (em
// projeto_final/bomberman.cpp) calcula cada amostra. Este programa faz a
// mesma conta e mostra os números; a Aula 23 monta o cabeçalho WAV e toca.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iterator>
#include <vector>

int main() {
    const int amostragem = 11025;          // amostras por segundo
    const double duracaoNota = 0.17;       // segundos
    // G4 A4 C5 A4 G4 F4 G4 C5, em hertz (vibrações por segundo).
    const double notas[] = {392.00, 440.00, 523.25, 440.00, 392.00, 349.23, 392.00, 523.25};
    int amostrasPorNota = static_cast<int>(amostragem * duracaoNota);

    std::vector<unsigned char> amostras;   // 8 bits sem sinal: 0..255, silêncio = 128
    amostras.reserve(amostrasPorNota * std::size(notas));
    for (double frequencia : notas) {
        for (int i = 0; i < amostrasPorNota; i++) {
            double t = static_cast<double>(i) / amostragem;          // tempo desde o início da nota
            double ciclo = std::fmod(t * frequencia, 1.0);           // posição dentro de um período (0..1)
            double onda = ciclo < 0.5 ? 1.0 : -1.0;                  // onda quadrada
            double envelope = 1.0 - (static_cast<double>(i) / amostrasPorNota) * 0.4;   // fade de 100% a 60%
            int amostra = static_cast<int>(128 + onda * envelope * 40);
            amostra = std::max(0, std::min(255, amostra));
            amostras.push_back(static_cast<unsigned char>(amostra));
        }
    }

    std::printf("amostras por nota=%d (11025 x 0,17)\n", amostrasPorNota);
    std::printf("total=%zu amostras = %zu bytes = %.2f s de musica\n",
                amostras.size(), amostras.size(), amostras.size() / double(amostragem));
    std::printf("periodo de G4: 11025 / 392 = %.1f amostras (metade alta, metade baixa)\n", amostragem / 392.0);

    // As 30 primeiras amostras da 1ª nota, desenhadas de lado: 168 = alto, 88 = baixo.
    std::printf("inicio da onda:\n");
    for (int i = 0; i < 30; i++) std::printf("  %3d %s\n", amostras[i], amostras[i] > 128 ? "########" : "#");
    std::printf("ultima amostra da 1a nota: %d (o fade baixou a amplitude)\n", amostras[amostrasPorNota - 1]);
    return 0;
}
