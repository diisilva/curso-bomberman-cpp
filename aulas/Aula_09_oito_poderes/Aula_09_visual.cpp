// Aula 09: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"escondido no bloco", L"chama revela", L"pisa e coleta", L"nova fase"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: escondido no bloco", L"blocosBonus = {(1,2) RAIO, (1,3) BOMBA_EXTRA, ...}", L"pisar em (1,2) não dá nada", L"Enquanto o bloco existe, o item só está no vetor blocosBonus.", 0, -1, -1},
    {L"Etapa 2: chama revela", L"destruirBlocoBonus(1,2) → itensNoChao.push_back", L"2 itens no chão, 1 escondido", L"O item muda de vetor: de escondido para visível.", 1, -1, -1},
    {L"Etapa 3: pisa e coleta", L"switch (tipo): RAIO → alcance++; BOMBA_EXTRA → extras++", L"alcance = 2, limite = 2, +60 pontos", L"Cada case altera só a variável do seu poder; RAIO tem teto 5.", 2, -1, -1},
    {L"Etapa 4: nova fase", L"montarFase: alcance = 1; bombasExtras = 0; ...", L"alcance = 1, limite = 1, vidas = 4", L"Poderes são da fase e zeram; vidas são da partida e ficam.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: posicionarBlocosBonus esconde 8 itens; a chama revela; pisar coleta; montarFase zera.");
    visual::mostrarItens(g);
}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 09: Os oito poderes", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 09: Os oito poderes";
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
