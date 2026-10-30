// Aula 02: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"borda + pilares", L"sorteio de caixas (36%)", L"limparArea(1,1) do jogo", L"limparArea corrigida"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: borda + pilares", L"borda: l==0, l==12, c==0, c==14; pilar: l%2==0 && c%2==0", L"30 pilares + a moldura de #", L"mapa[l][c]: o 1º índice é a linha (desce), o 2º é a coluna (vai à direita).", 0, 2, 2},
    {L"Etapa 2: sorteio de caixas (36%)", L"chance(gerador) < 36 em cada uma das 113 casas livres", L"43 caixas (+) com a semente 2026", L"PAREDE_FRAGIL é um valor do enum, não um número mágico.", 1, 1, 1},
    {L"Etapa 3: limparArea(1,1) do jogo", L"mapa[0][1] = VAZIO; mapa[1][0] = VAZIO", L"buracos na borda: borda intacta = 0", L"O jogo real abre as quatro vizinhas sem olhar se são parede.", 2, 0, 1},
    {L"Etapa 4: limparArea corrigida", L"só abre a vizinha se for PAREDE_FRAGIL", L"borda intacta = 1", L"Exercício 2: a correção está em solucoes/.", 3, 0, 1}
};
const char* mapa0[] = {"###############", "#.............#", "#.#.#.#.#.#.#.#", "#.............#", "#.#.#.#.#.#.#.#", "#.............#", "#.#.#.#.#.#.#.#", "#.............#", "#.#.#.#.#.#.#.#", "#.............#", "#.#.#.#.#.#.#.#", "#.............#", "###############"};
const char* mapa1[] = {"###############", "#+.....+..+..+#", "#.#.#+#.#+#.#.#", "#+...+...+....#", "#+#.#+#.#+#.#+#", "#++++++..+...+#", "#.#+#.#.#+#.#.#", "#.......++....#", "#.#.#+#+#.#.#+#", "#.++.+.+.++++.#", "#+#.#+#.#.#+#.#", "#+...++..++...#", "###############"};
const char* mapa2[] = {"#.#############", ".S.....+..+..+#", "#.#.#+#.#+#.#.#", "#+...+...+....#", "#+#.#+#.#+#.#+#", "#++++++..+...+#", "#.#+#.#.#+#.#.#", "#.......++....#", "#.#.#+#+#.#.#+#", "#.++.+.+.++++.#", "#+#.#+#.#.#+#.#", "#+...++..++...#", "###############"};
const char* mapa3[] = {"###############", "#S.....+..+..+#", "#.#.#+#.#+#.#.#", "#+...+...+....#", "#+#.#+#.#+#.#+#", "#++++++..+...+#", "#.#+#.#.#+#.#.#", "#.......++....#", "#.#.#+#+#.#.#+#", "#.++.+.+.++++.#", "#+#.#+#.#.#+#.#", "#+...++..++...#", "###############"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 13, 15, nullptr, L"No jogo: criarMapa monta a arena 13 × 15; limparArea abre espaço para quem nasce.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 02: Matriz, enum e mapa", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 02: Matriz, enum e mapa";
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
