// Aula 17: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"WinMain", L"WM_TIMER", L"WM_PAINT", L"WM_DESTROY"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: WinMain", L"DPI → GDI+ → RegisterClassW → CreateWindowExW → laço", L"janela criada; o jogo espera", L"Comece pelo fim do arquivo: é onde o programa começa.", 0, -1, -1},
    {L"Etapa 2: WM_TIMER", L"tempo real → atualizar(8) × n → InvalidateRect", L"o estado avança em passos de 8 ms", L"atualizar chama 25 funções: é o índice das regras.", 1, -1, -1},
    {L"Etapa 3: WM_PAINT", L"desenhar: buffer → menu ou partida → BitBlt", L"um quadro completo na tela", L"O desenho só lê o estado; nunca decide regra.", 2, -1, -1},
    {L"Etapa 4: WM_DESTROY", L"KillTimer → pararMusica → encerrarControle → liberarAssets", L"recursos liberados em ordem", L"Quem cria cada recurso também sabe quando liberá-lo.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: 4450 linhas e 242 funções; quatro mensagens organizam tudo.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 17: Leitura guiada do jogo real", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 17: Leitura guiada do jogo real";
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
