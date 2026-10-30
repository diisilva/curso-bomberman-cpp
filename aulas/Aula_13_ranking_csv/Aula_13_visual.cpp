// Aula 13: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"5 campos por linha", L"1º critério: pontos", L"bombas e passos", L"gravar e reler"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: 5 campos por linha", L"data;jogador;pontos;bombas;movimentos", L"14/10/2026;Bia;900;3;35", L"CSV é texto puro: dá para abrir no Bloco de Notas e conferir.", 0, -1, -1},
    {L"Etapa 2: 1º critério: pontos", L"Davi, Bia e Ana: 900 > Caio: 700", L"Caio fica em último", L"Mais pontos vence; o resto só desempata.", 1, -1, -1},
    {L"Etapa 3: bombas e passos", L"Davi 2 bombas < 3; Bia 35 passos < Ana 40", L"Davi, Bia, Ana", L"Menos bombas e menos passos: jogar com eficiência conta.", 2, -1, -1},
    {L"Etapa 4: gravar e reler", L"ofstream << ...; getline(partes, campo, ';')", L"1. Davi  2. Bia  3. Ana  4. Caio", L"Ler de volta o que foi gravado é o teste mais simples de um formato.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: ranking.csv fica ao lado do .exe; a tela 6 do menu lê, ordena e mostra.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 13: Ranking em CSV", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 13: Ranking em CSV";
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
