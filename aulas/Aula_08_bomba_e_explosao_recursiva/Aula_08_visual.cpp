// Aula 08: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"t=0: colocarBombaEm (2,1)", L"t=2200: direita, 1ª casa", L"direita, 2ª e 3ª casas", L"cima, baixo e esquerda"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: t=0: colocarBombaEm (2,1)", L"nova.tempoMs = TEMPO_BOMBA_MS = 2200", L"fusível armado: 275 atualizações", L"atualizarBombas desconta 8 ms por passo até chegar a zero.", 0, 2, 1},
    {L"Etapa 2: t=2200: direita, 1ª casa", L"propagar(0,+1, alcance 3): (2,2) é caixa", L"caixa vira chão; segue com alcance 2", L"Caixa não é caso base: a chama quebra e continua.", 1, 2, 2},
    {L"Etapa 3: direita, 2ª e 3ª casas", L"(2,3) caixa → alcance 1; (2,4) chão → alcance 0", L"alcance 0: caso base, para", L"Cada chamada recebe alcanceRestante - 1: a recursão sempre termina.", 2, 2, 4},
    {L"Etapa 4: cima, baixo e esquerda", L"(1,1) e (3,1) chão; (2,0) é '#': caso base", L"6 células com chama; 2 caixas", L"Parede sólida para a chama sem entrar na lista de células.", 3, 2, 0}
};
const char* mapa0[] = {"#######", "#.....#", "#B++..#", "#.....#", "#######"};
const char* mapa1[] = {"#######", "#.....#", "#B*+..#", "#.....#", "#######"};
const char* mapa2[] = {"#######", "#.....#", "#B***.#", "#.....#", "#######"};
const char* mapa3[] = {"#######", "#*....#", "#B***.#", "#*....#", "#######"};
const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Grade, itens, etapas, mapas[passo], 5, 7, nullptr, L"No jogo: propagarExplosao é chamada 4 vezes; cada chamada avança uma casa e chama a si mesma.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 08: Bomba e explosão recursiva", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 08: Bomba e explosão recursiva";
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
