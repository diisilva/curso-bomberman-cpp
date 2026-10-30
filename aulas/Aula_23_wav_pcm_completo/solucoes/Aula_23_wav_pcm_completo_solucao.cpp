// Aula 23 — solução do exercício 3: ler e validar um WAV.
//
// O inverso de gerarMusicaProcedural: abrir um arquivo, conferir as quatro
// marcas (RIFF, WAVE, "fmt ", data), conferir se os tamanhos batem e só
// então confiar nos números. Um arquivo cortado ao meio precisa ser recusado.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#pragma pack(push, 1)
struct CabecalhoWav {
    char riff[4]; uint32_t tamanhoArquivo; char onda[4]; char fmt[4]; uint32_t tamanhoFmt;
    uint16_t formato, canais; uint32_t amostragem, bytesPorSegundo; uint16_t alinhamentoBloco, bitsPorAmostra;
    char dataTag[4]; uint32_t tamanhoDados;
};
#pragma pack(pop)

bool marca(const char* campo, const char* esperado) { return std::memcmp(campo, esperado, 4) == 0; }

std::string validar(const std::vector<unsigned char>& bytes) {
    if (bytes.size() < sizeof(CabecalhoWav)) return "menor que um cabecalho";
    CabecalhoWav h;
    std::memcpy(&h, bytes.data(), sizeof(h));
    if (!marca(h.riff, "RIFF") || !marca(h.onda, "WAVE")) return "nao e RIFF/WAVE";
    if (!marca(h.fmt, "fmt ") || !marca(h.dataTag, "data")) return "blocos fmt/data fora do lugar";
    if (h.formato != 1) return "nao e PCM";
    if (h.bytesPorSegundo != h.amostragem * h.canais * (h.bitsPorAmostra / 8)) return "bytesPorSegundo incoerente";
    if (h.tamanhoArquivo != bytes.size() - 8) return "tamanhoArquivo nao bate com o arquivo";
    if (sizeof(CabecalhoWav) + h.tamanhoDados > bytes.size()) return "faltam dados (arquivo cortado)";
    char texto[96];
    std::snprintf(texto, sizeof texto, "ok: %u Hz, %u bits, %u canal(is), %.2f s",
                  h.amostragem, h.bitsPorAmostra, h.canais, h.tamanhoDados / double(h.bytesPorSegundo));
    return texto;
}

std::vector<unsigned char> lerArquivo(const char* caminho) {
    std::ifstream arquivo(caminho, std::ios::binary);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(arquivo), {});
}

int main() {
    // Gera um WAV pequeno (meio segundo de silêncio) no mesmo formato do jogo.
    CabecalhoWav h{{'R','I','F','F'}, 0, {'W','A','V','E'}, {'f','m','t',' '}, 16, 1, 1, 11025, 11025, 1, 8, {'d','a','t','a'}, 5512};
    h.tamanhoArquivo = sizeof(h) - 8 + h.tamanhoDados;
    std::vector<unsigned char> wav(sizeof(h) + h.tamanhoDados, 128);
    std::memcpy(wav.data(), &h, sizeof(h));
    std::ofstream("teste_ok.wav", std::ios::binary).write(reinterpret_cast<const char*>(wav.data()), static_cast<std::streamsize>(wav.size()));
    std::ofstream("teste_cortado.wav", std::ios::binary).write(reinterpret_cast<const char*>(wav.data()), 2000);

    for (const char* nome : {"teste_ok.wav", "teste_cortado.wav", "som_aula.wav"})
        std::printf("%-18s %s\n", nome, validar(lerArquivo(nome)).c_str());
    return 0;
}
