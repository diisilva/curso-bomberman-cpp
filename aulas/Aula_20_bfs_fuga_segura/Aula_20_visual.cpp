// Aula 20: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"vale plantar aqui?", L"alcance 1: há fuga", L"alcance 2: sem fuga", L"o prazo da fuga"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: vale plantar aqui?", L"alvoParaBomba((1,2)): caixa em (1,1), no alcance", L"sim: há caixa para quebrar", L"Sem alvo, plantar é desperdício de bomba.", 0, 1, 1},
    {L"Etapa 2: alcance 1: há fuga", L"(1,3) na chama; (1,4) fora dela, a 2 passos", L"fuga em 2 passos → PLANTA", L"A BFS para na primeira casa onde a previsão diz 'seguro'.", 1, 1, 4},
    {L"Etapa 3: alcance 2: sem fuga", L"(1,3) e (1,4) na chama; o corredor acaba", L"nenhuma casa segura → não planta", L"Plantar ali seria suicídio: o bot espera ou procura outro lugar.", 2, 1, 4},
    {L"Etapa 4: o prazo da fuga", L"2200 ms / 280 ms = 7 passos (com 440 ms: 5)", L"casa a 6 passos só vale com prazo 7", L"Não basta existir caminho: é preciso chegar antes da explosão.", 3, 1, 4}
};
const char* mapa0[] = {"#######", "#+S..##", "#######"};
const char* mapa1[] = {"#######", "#*B*F##", "#######"};
const char* mapa2[] = {"#######", "#*B**##", "#######"};
const char* mapa3[] = {"#######", "#*B*F##", "#######"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 3, 7, nullptr, L"No jogo: o bot só planta se alvoParaBomba E existeFugaSegura forem verdadeiros.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 20: BFS de fuga segura", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 20: BFS de fuga segura";
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
