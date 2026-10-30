// Aula 10: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"origem entra na fila", L"ondas de distância 1 e 2", L"contorna a parede", L"volta pelo anterior"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: origem entra na fila", L"fila = {(1,1)}; visitado[1][1] = true", L"distância 0", L"A busca começa no inimigo (S) e procura o jogador (F).", 0, 1, 1},
    {L"Etapa 2: ondas de distância 1 e 2", L"sai (1,1) → entram (2,1) e (1,2); depois (2,2), (3,1)", L"perto é visitado antes de longe", L"FIFO: quem entrou primeiro sai primeiro, então a busca anda em ondas.", 1, 2, 1},
    {L"Etapa 3: contorna a parede", L"coluna 3 bloqueada nas linhas 1 e 2", L"alvo alcançado a 8 passos", L"anterior[][] guarda de onde cada casa foi descoberta.", 2, 3, 3},
    {L"Etapa 4: volta pelo anterior", L"(1,5) ← (1,4) ← (2,4) ← (3,4) ← ... ← (2,1)", L"primeiro passo = (2,1)", L"O jogo só usa o 1º passo: na próxima decisão, recalcula tudo.", 3, 2, 1}
};
const char* mapa0[] = {"#######", "#S.#.F#", "#..#..#", "#.....#", "#######"};
const char* mapa1[] = {"#######", "#So#.F#", "#oo#..#", "#o....#", "#######"};
const char* mapa2[] = {"#######", "#So#oF#", "#oo#oo#", "#ooooo#", "#######"};
const char* mapa3[] = {"#######", "#S.#oF#", "#o.#o.#", "#oooo.#", "#######"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 5, 7, nullptr, L"No jogo: a mesma bfsProximoPasso guia inimigos, chefe e bot; só mudam as duas lambdas.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 10: Fila e busca em largura", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 10: Fila e busca em largura";
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
