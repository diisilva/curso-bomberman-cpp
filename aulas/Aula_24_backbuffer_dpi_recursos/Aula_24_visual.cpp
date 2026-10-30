// Aula 24: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"WM_SIZE: escala", L"garantirBuffer", L"desenha no buffer", L"BitBlt"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: WM_SIZE: escala", L"janela 900 × 480: min(900/560, 480/420) = 1,14", L"escala 1,14; margem x = 130", L"A menor razão cabe nos dois eixos; a sobra vira faixa lateral.", 0, -1, -1},
    {L"Etapa 2: garantirBuffer", L"mesmo tamanho? reaproveita; mudou? recria", L"um bitmap por tamanho de janela", L"Criar um bitmap a cada quadro desperdiça tempo e memória.", 1, -1, -1},
    {L"Etapa 3: desenha no buffer", L"Graphics g(bufferDC); Translate(margem); px(...)", L"quadro pronto, fora da tela", L"px() converte as medidas do jogo para pixels da janela.", 2, -1, -1},
    {L"Etapa 4: BitBlt", L"BitBlt(destino, 0, 0, w, h, bufferDC, 0, 0, SRCCOPY)", L"o quadro chega inteiro", L"A tela nunca mostra meio desenho: acabou o pisca-pisca.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: tudo é desenhado em 1080 × 852 e escalado para a janela e o monitor.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 24: Backbuffer, escala e DPI", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 24: Backbuffer, escala e DPI";
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
