// Aula 01: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"Posicao{1,1}", L"moverErrado(cópia)", L"mover(&)", L"avancarVisual"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: Posicao{1,1}", L"struct Posicao { int linha, coluna; };", L"jogador = (1,1)", L"Duas variáveis que sempre andam juntas viram um tipo só.", 0, -1, -1},
    {L"Etapa 2: moverErrado(cópia)", L"moverErrado(jogador, 0, 1)", L"jogador continua (1,1)", L"Sem &, a função alterou uma cópia que sumiu ao terminar.", 1, -1, -1},
    {L"Etapa 3: mover(&)", L"mover(jogador, 0, 1)", L"jogador = (1,2)", L"Com &, alvo é outro nome para o jogador de main.", 2, -1, -1},
    {L"Etapa 4: avancarVisual", L"3,6 células/s × 0,1 s = 0,36 célula", L"visual = (1; 1,36)", L"A célula lógica já é (1,2); o desenho chega lá aos poucos.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: moverJogador escolhe a célula; avancarVisual desliza o sprite até ela.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 01: Tipos, funções e referências", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 01: Tipos, funções e referências";
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
