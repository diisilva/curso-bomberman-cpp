#define WinMain Aula18WinMain
#include "../aulas/Aula_18_animacao_direcional_gdiplus/Aula_18_animacao_direcional_gdiplus.cpp"
#include "../visual/visual_base.h"
#include <cassert>
int main() {
    Gdiplus::GdiplusStartupInput entrada; ULONG_PTR sessao=0;
    if (Gdiplus::GdiplusStartup(&sessao,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    if (!carregarFolha()) return 2;
    if (origemAtual().X!=131 || origemAtual().Y!=38) return 3;
    {
        Gdiplus::Bitmap imagem(LARGURA,ALTURA); Gdiplus::Graphics g(&imagem);
        desenharCena(g);
        if (!visual::salvarPng(imagem,L"aulas\\Aula_18_animacao_direcional_gdiplus\\imagens\\aula18_inicio.png")) return 4;
    }
    logicaC=2; direcao=DIREITA;
    for (int i=0;i<6;++i) atualizarMovimento();
    if (!(visualC>1.0f && visualC<2.0f) || origemAtual().X<186) return 5;
    {
        Gdiplus::Bitmap imagem(LARGURA,ALTURA); Gdiplus::Graphics g(&imagem);
        desenharCena(g);
        if (!visual::salvarPng(imagem,L"aulas\\Aula_18_animacao_direcional_gdiplus\\imagens\\aula18_movimento.png")) return 6;
    }
    for (int i=0;i<20;++i) atualizarMovimento();
    if (visualC!=2.0f || indiceCiclo!=0) return 7;
    folha.reset(); Gdiplus::GdiplusShutdown(sessao); return 0;
}
