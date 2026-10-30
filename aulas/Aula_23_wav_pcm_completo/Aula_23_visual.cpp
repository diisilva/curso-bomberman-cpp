// Aula 23: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"RIFF / WAVE", L"bloco fmt", L"bloco data", L"PlaySoundW"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: RIFF / WAVE", L"'RIFF' + tamanhoArquivo + 'WAVE'", L"tamanhoArquivo = 15036 − 8 = 15028", L"O primeiro bloco diz que tipo de arquivo é e quanto falta ler.", 0, -1, -1},
    {L"Etapa 2: bloco fmt", L"formato 1 (PCM), 1 canal, 11025 Hz, 8 bits", L"bytesPorSegundo = 11025", L"Sem estes números, os bytes seguintes seriam só lixo.", 1, -1, -1},
    {L"Etapa 3: bloco data", L"'data' + tamanhoDados (14992) + as amostras", L"cabeçalho de 44 bytes", L"#pragma pack(1) garante 44 bytes sem enchimento.", 2, -1, -1},
    {L"Etapa 4: PlaySoundW", L"SND_MEMORY | SND_ASYNC | SND_LOOP", L"o buffer precisa viver enquanto toca", L"Por isso bufferMusica é global, e pararMusica vem antes de liberar.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: o WAV é montado na memória e entregue ao PlaySoundW, que toca em loop.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 23: Arquivo WAV completo", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 23: Arquivo WAV completo";
    bool ok=true;
    for (int passo : {0,1,2,3}) {
        Gdiplus::Bitmap imagem(visual::LARGURA, visual::ALTURA);
        Gdiplus::Graphics g(&imagem);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        desenharAula(g,passo,0);
        wchar_t caminho[80];
        swprintf(caminho,80,L"imagens\\etapa_%d.png",passo+1);
        if (!visual::salvarPng(imagem,caminho)) ok=false;
    }
    visual::liberarAssets();
    Gdiplus::GdiplusShutdown(token);
    return ok?0:2;
}
#endif
