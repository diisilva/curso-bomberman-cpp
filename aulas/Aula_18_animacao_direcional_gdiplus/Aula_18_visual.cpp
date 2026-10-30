// Aula 18: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"direção → linha", L"tabela de Rect", L"ciclo {0,1,0,2}", L"DrawImage"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: direção → linha", L"baixo 0, direita 1, cima 2, esquerda 3", L"uma linha da folha por direção", L"A direção não depende do quadro atual da caminhada.", 0, -1, -1},
    {L"Etapa 2: tabela de Rect", L"Rect x[d][p], y = 38, altura = 26 (Aula 05)", L"recorte irregular medido", L"A folha desta aula não é uma grade regular de quadro × 16.", 1, -1, -1},
    {L"Etapa 3: ciclo {0,1,0,2}", L"pose = ciclo[int(distância × 4) % 4]", L"pose troca a cada 0,25 célula", L"A pose segue a distância andada: parado, volta à pose neutra.", 2, -1, -1},
    {L"Etapa 4: DrawImage", L"origem = Rect da folha; destino = célula ampliada", L"sprite ampliado, sem o fundo roxo", L"SetColorKey(112,0,170) apaga só a cor de fundo do recorte.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: linhaDaFolhaParaDirecao escolhe a linha; quadroCaminhada escolhe a coluna da folha.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 18: Animação direcional real", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 18: Animação direcional real";
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
