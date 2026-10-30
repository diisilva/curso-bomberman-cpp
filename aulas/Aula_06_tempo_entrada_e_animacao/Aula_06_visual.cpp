// Aula 06: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"chega 16 ms", L"chega 40 ms", L"chega 16 ms", L"chega 250 ms"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: chega 16 ms", L"acumulado = 0 + 16 = 16  →  16 / 8 = 2 passos", L"total = 2, resto = 0", L"Cada atualizar(8) consome exatamente 8 ms de simulação.", 0, -1, -1},
    {L"Etapa 2: chega 40 ms", L"acumulado = 0 + 40 = 40  →  5 passos", L"total = 7, resto = 0", L"Um tick atrasado não perde tempo: roda mais passos de uma vez.", 1, -1, -1},
    {L"Etapa 3: chega 16 ms", L"acumulado = 0 + 16  →  2 passos", L"total = 9, resto = 0", L"A soma dos ms recebidos vira sempre a mesma simulação.", 2, -1, -1},
    {L"Etapa 4: chega 250 ms", L"min(250, 100) = 100  →  12 passos, sobram 4 ms", L"total = 21, resto = 4", L"O teto de 100 ms evita uma avalanche de passos depois de uma pausa.", 3, -1, -1}
};
const int valores[4] = {2, 7, 9, 21};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Barras, itens, etapas, nullptr, 0, 0, valores, L"No jogo: WM_TIMER mede o tempo real e chama atualizar(8) quantas vezes couber.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 06: Tempo, entrada e animação", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 06: Tempo, entrada e animação";
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
