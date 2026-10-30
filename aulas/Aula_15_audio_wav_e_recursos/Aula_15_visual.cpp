// Aula 15: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"amostragem", L"onda quadrada", L"envelope", L"8 bits sem sinal"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: amostragem", L"11025 amostras/s × 0,17 s por nota", L"1874 amostras por nota", L"Som digital é uma lista de números medidos muitas vezes por segundo.", 0, -1, -1},
    {L"Etapa 2: onda quadrada", L"fmod(t × 392, 1) < 0,5 ? +1 : −1", L"alto/baixo a cada ~14 amostras", L"A frequência decide quão rápido a onda alterna: isso é a nota.", 1, -1, -1},
    {L"Etapa 3: envelope", L"1 − 0,4 × i / 1874", L"volume cai de 100% para 60%", L"O fade no fim da nota evita o estalo na troca de nota.", 2, -1, -1},
    {L"Etapa 4: 8 bits sem sinal", L"128 + onda × envelope × 40", L"14992 bytes = 1,36 s", L"Silêncio é 128; 168 e 88 são os picos da onda.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: gerarMusicaProcedural calcula a música inteira; não há arquivo de áudio.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 15: Áudio: amostras PCM", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 15: Áudio: amostras PCM";
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
