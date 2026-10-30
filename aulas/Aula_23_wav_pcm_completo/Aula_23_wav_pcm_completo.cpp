// Aula 23 — Arquivo WAV completo e reprodução
//
// CabecalhoWav e gerarMusicaProcedural são os do jogo (projeto_final/bomberman.cpp).
// O programa monta o WAV inteiro na memória, como o jogo, grava uma cópia em
// som_aula.wav (para você abrir num player) e toca a partir da memória.
// Defina CURSO_BOMBERMAN_SEM_SOM=1 para pular o som (a validação faz isso).
#include <windows.h>
#include <mmsystem.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

// pack(1): sem bytes de enchimento entre os campos; o formato exige 44 bytes exatos.
#pragma pack(push, 1)
struct CabecalhoWav {
    char riff[4] = {'R', 'I', 'F', 'F'};
    uint32_t tamanhoArquivo = 0;          // tudo depois destes 8 primeiros bytes
    char onda[4] = {'W', 'A', 'V', 'E'};
    char fmt[4] = {'f', 'm', 't', ' '};
    uint32_t tamanhoFmt = 16;
    uint16_t formato = 1;                 // 1 = PCM, sem compressão
    uint16_t canais = 1;
    uint32_t amostragem = 11025;
    uint32_t bytesPorSegundo = 11025;     // amostragem × canais × bytes por amostra
    uint16_t alinhamentoBloco = 1;        // canais × bytes por amostra
    uint16_t bitsPorAmostra = 8;
    char dataTag[4] = {'d', 'a', 't', 'a'};
    uint32_t tamanhoDados = 0;
};
#pragma pack(pop)
static_assert(sizeof(CabecalhoWav) == 44, "o cabecalho WAV PCM tem 44 bytes");

std::vector<unsigned char> gerarMusicaProcedural() {
    const int amostragem = 11025;
    const double duracaoNota = 0.17;
    const double notas[] = {392.00, 440.00, 523.25, 440.00, 392.00, 349.23, 392.00, 523.25};
    int amostrasPorNota = static_cast<int>(amostragem * duracaoNota);
    std::vector<unsigned char> amostras;
    amostras.reserve(amostrasPorNota * std::size(notas));
    for (double frequencia : notas)
        for (int i = 0; i < amostrasPorNota; i++) {
            double t = static_cast<double>(i) / amostragem;
            double onda = std::fmod(t * frequencia, 1.0) < 0.5 ? 1.0 : -1.0;
            double envelope = 1.0 - (static_cast<double>(i) / amostrasPorNota) * 0.4;
            int amostra = std::max(0, std::min(255, static_cast<int>(128 + onda * envelope * 40)));
            amostras.push_back(static_cast<unsigned char>(amostra));
        }

    CabecalhoWav cabecalho;
    cabecalho.tamanhoDados = static_cast<uint32_t>(amostras.size());
    cabecalho.tamanhoArquivo = static_cast<uint32_t>(sizeof(CabecalhoWav) - 8 + amostras.size());
    std::vector<unsigned char> arquivo(sizeof(CabecalhoWav) + amostras.size());
    std::memcpy(arquivo.data(), &cabecalho, sizeof(CabecalhoWav));                       // 44 bytes de cabeçalho
    std::memcpy(arquivo.data() + sizeof(CabecalhoWav), amostras.data(), amostras.size()); // e logo depois, o som
    return arquivo;
}

// No jogo, este vetor é global: o PlaySound lê dele enquanto toca.
std::vector<unsigned char> bufferMusica;

int main() {
    bufferMusica = gerarMusicaProcedural();
    CabecalhoWav lido;
    std::memcpy(&lido, bufferMusica.data(), sizeof(lido));
    std::printf("cabecalho=%zu bytes, dados=%u bytes, arquivo=%zu bytes\n",
                sizeof(CabecalhoWav), lido.tamanhoDados, bufferMusica.size());
    std::printf("campo tamanhoArquivo=%u (= arquivo - 8)\n", lido.tamanhoArquivo);
    std::printf("duracao=%.2f s (dados / bytesPorSegundo)\n", lido.tamanhoDados / double(lido.bytesPorSegundo));

    std::ofstream copia("som_aula.wav", std::ios::binary | std::ios::trunc);
    copia.write(reinterpret_cast<const char*>(bufferMusica.data()), static_cast<std::streamsize>(bufferMusica.size()));
    copia.close();
    std::printf("gravado som_aula.wav (abra num player para ouvir de novo)\n");

    if (!std::getenv("CURSO_BOMBERMAN_SEM_SOM")) {
        // O jogo usa SND_ASYNC | SND_LOOP: toca para sempre em segundo plano.
        // Aqui, SND_SYNC toca uma vez e só então devolve o controle.
        PlaySoundW(reinterpret_cast<LPCWSTR>(bufferMusica.data()), nullptr, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
    }
    return 0;
}
