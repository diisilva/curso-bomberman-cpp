// Aula 14: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"vida do chefe", L"bomba acerta", L"mesma chama de novo", L"subpassos de 25 ms"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: vida do chefe", L"round(48 × 1,15) = 55", L"Golem começa com 55", L"FATOR_FORCA_BOSS ajusta os cinco chefes de uma vez.", 0, -1, -1},
    {L"Etapa 2: bomba acerta", L"vida = max(0, 55 - 4); imunidadeMs = 700", L"vida = 51", L"Depois de um acerto, o chefe fica 0,7 s imune.", 1, -1, -1},
    {L"Etapa 3: mesma chama de novo", L"b.bossAtingido == true → return", L"vida continua 51", L"A chama dura 650 ms: sem a trava, uma bomba acertaria 81 vezes.", 2, -1, -1},
    {L"Etapa 4: subpassos de 25 ms", L"12 células/s × 0,025 s = 0,3 célula por teste", L"acerta em c = 3,10", L"Sem subpassos, um quadro de 250 ms pula 3 células de uma vez.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: fases pares têm chefe; bombas tiram 4 de vida e o chefe responde com projéteis.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 14: Chefes, projéteis e Mario", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 14: Chefes, projéteis e Mario";
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
