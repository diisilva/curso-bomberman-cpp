// Aula 11: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"bomba e duas caixas", L"previsão para na 1ª caixa", L"chama real atravessa", L"previsão corrigida"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: bomba e duas caixas", L"bomba (1,1), alcance 4, caixas em (1,2) e (1,3)", L"pergunta: (1,4) é seguro?", L"Antes de fugir, o bot precisa saber onde a chama vai chegar.", 0, 1, 4},
    {L"Etapa 2: previsão para na 1ª caixa", L"i = 1 é caixa → return i == distancia (1 == 3)", L"prevista = 0: \"seguro\"", L"celulaNoAlcance trata caixa como fim da chama.", 1, 1, 2},
    {L"Etapa 3: chama real atravessa", L"propagarExplosao: caixa quebra e a recursão continua", L"real = 1: (1,4) pega fogo", L"Mesma pergunta, duas regras: o bot pode fugir para dentro da chama.", 2, 1, 4},
    {L"Etapa 4: previsão corrigida", L"só PAREDE_SOLIDA interrompe a previsão", L"2925 comparações, 0 divergências", L"Exercício 2: uma regra só, testada em todas as casas.", 3, 1, 4}
};
const char* mapa0[] = {"#######", "#B++..#", "#######"};
const char* mapa1[] = {"#######", "#B++..#", "#######"};
const char* mapa2[] = {"#######", "#B****#", "#######"};
const char* mapa3[] = {"#######", "#B****#", "#######"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 3, 7, nullptr, L"No jogo: o bot usa celulaNoAlcance para prever a chama; a chama real é propagarExplosao.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 11: Perigo e fuga do bot", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 11: Perigo e fuga do bot";
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
