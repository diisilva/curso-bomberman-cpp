// Aula 12: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"novaPartida", L"morreu: reiniciar", L"entrou no portal", L"avancarFase"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: novaPartida", L"pontos = 0; vidas = 3; estatisticas = {}; montarFase()", L"fase 1, tudo zerado", L"Só novaPartida zera pontos e estatísticas da partida.", 0, -1, -1},
    {L"Etapa 2: morreu: reiniciar", L"vidas-- (em atualizar) → reiniciarFase() → montarFase()", L"vidas 2; pontos e movFase ficam", L"Repetir a fase custa uma vida e os bônus da fase.", 1, -1, -1},
    {L"Etapa 3: entrou no portal", L"max(0, 500 - 70 - 5 × 10) = 380", L"+380 pontos de bônus", L"Eficiência: cada passo custa 1 e cada bomba 5, somando as tentativas.", 2, -1, -1},
    {L"Etapa 4: avancarFase", L"fase++; vidas = 3; movFase = bombasFase = 0", L"fase 2, vidas 3, 680 pontos", L"Vidas voltam à base: uma VIDA_EXTRA não passa de fase.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: três funções trocam de fase, e cada uma preserva coisas diferentes.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 12: Fases, dificuldade e dois jogadores", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 12: Fases, dificuldade e dois jogadores";
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
