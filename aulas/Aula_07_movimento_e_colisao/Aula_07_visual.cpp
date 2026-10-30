// Aula 07: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"direita: reserva (1,2)", L"80 ms: baixo é ignorado", L"280 ms: chegou", L"baixo: pilar em (2,2)"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: direita: reserva (1,2)", L"podeAndarPara((1,2)) → '.' livre", L"lógica = (1,2), visual = (1; 1,00)", L"A casa fica reservada antes de o sprite sair do lugar.", 0, 1, 2},
    {L"Etapa 2: 80 ms: baixo é ignorado", L"visualNoDestino = falso; tempoMovimentoMs = 200", L"visual = (1; 1,29), sem curva", L"Curvas só no centro da casa: nada de cortar quina.", 1, 1, 2},
    {L"Etapa 3: 280 ms: chegou", L"3,6 células/s × 0,28 s = 1,008 ≥ 1", L"visual = (1; 2,00), chegou = 1", L"O cooldown de 280 ms termina junto com a chegada.", 2, 1, 2},
    {L"Etapa 4: baixo: pilar em (2,2)", L"mapa[2][2] = '#' → podeAndarPara falso", L"recusado: lógica continua (1,2)", L"Passo recusado não gasta cooldown: outra direção pode tentar já.", 3, 2, 2}
};
const char* mapa0[] = {"#####", "#So.#", "#.#.#", "#...#", "#####"};
const char* mapa1[] = {"#####", "#So.#", "#.#.#", "#...#", "#####"};
const char* mapa2[] = {"#####", "#.S.#", "#.#.#", "#...#", "#####"};
const char* mapa3[] = {"#####", "#.S.#", "#.#.#", "#...#", "#####"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 5, 5, nullptr, L"No jogo: moverJogador reserva a casa na hora; avancarVisual leva o desenho até ela.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 07: Movimento e colisão", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 07: Movimento e colisão";
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
