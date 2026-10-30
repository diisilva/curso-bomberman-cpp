// Aula 19: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"bomba à frente", L"chute: até 4 casas", L"teleporte", L"atravessar caixas"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: bomba à frente", L"esperado = jogador + direcaoJogador = (1,2)", L"há bomba: o chute vale", L"O chute só procura a bomba na casa para onde o jogador olha.", 0, 1, 2},
    {L"Etapa 2: chute: até 4 casas", L"(1,3), (1,4), (1,5), (1,6) livres; ALCANCE_CHUTE = 4", L"bomba em (1,6); carta gasta", L"Para antes de parede, caixa ou inimigo; nunca passa por cima.", 1, 1, 6},
    {L"Etapa 3: teleporte", L"sorteia entre casas livres, sem inimigo nem bomba", L"jogador e desenho em (1,2)", L"O desenho salta junto: deslizar cruzaria o mapa inteiro.", 2, 1, 2},
    {L"Etapa 4: atravessar caixas", L"atravessa && mapa == PAREDE_FRAGIL", L"caixa = 1; bloco bônus = 0; parede = 0", L"A carta não deixa pegar itens escondidos andando por dentro.", 3, 3, 5}
};
const char* mapa0[] = {"#########", "#SB.....#", "#.#.#.#.#", "#....+..#", "#########"};
const char* mapa1[] = {"#########", "#S....B.#", "#.#.#.#.#", "#....+..#", "#########"};
const char* mapa2[] = {"#########", "#.S...B.#", "#.#.#.#.#", "#....+..#", "#########"};
const char* mapa3[] = {"#########", "#.S...B.#", "#.#.#.#.#", "#....+..#", "#########"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 5, 9, nullptr, L"No jogo: E chuta, T teleporta, F detona; imunidade e travessia mudam as regras de dano e de passagem.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 19: Motor completo dos poderes", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 19: Motor completo dos poderes";
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
