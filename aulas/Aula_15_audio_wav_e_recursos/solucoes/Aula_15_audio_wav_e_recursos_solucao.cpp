// Aula 15 — solução dos exercícios 2 e 3: o que muda o tamanho do áudio?
//
// bytes = amostragem × segundos × canais × (bits / 8)
// A FREQUÊNCIA das notas não aparece na conta: uma oitava acima (o dobro da
// frequência) soa mais aguda, mas ocupa exatamente os mesmos bytes.
#include <cstdio>

long long bytesDeAudio(int amostragem, double segundos, int canais, int bits) {
    long long amostrasPorCanal = static_cast<long long>(amostragem * segundos);
    return amostrasPorCanal * canais * (bits / 8);
}

int main() {
    const double segundos = 8 * static_cast<int>(11025 * 0.17) / 11025.0;   // as 8 notas do jogo
    struct Formato { const char* nome; int amostragem, canais, bits; };
    const Formato formatos[] = {
        {"jogo: 11025 Hz, mono, 8 bits ", 11025, 1, 8},
        {"16 bits                       ", 11025, 1, 16},
        {"qualidade de CD: 44100, estereo, 16 bits", 44100, 2, 16},
    };
    for (const Formato& f : formatos)
        std::printf("%s -> %lld bytes\n", f.nome, bytesDeAudio(f.amostragem, segundos, f.canais, f.bits));

    // Exercício 3: uma oitava acima. O período em amostras cai pela metade.
    std::printf("G4 (392 Hz): periodo de %.1f amostras; G5 (784 Hz): %.1f amostras; mesmos bytes\n",
                11025 / 392.0, 11025 / 784.0);
    return 0;
}
