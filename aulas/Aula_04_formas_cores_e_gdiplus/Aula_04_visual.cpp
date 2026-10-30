// Aula 04: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"InvalidateRect", L"BeginPaint → HDC", L"Graphics + pincel", L"EndPaint"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: InvalidateRect", L"InvalidateRect(janela, nullptr, FALSE)", L"WM_PAINT agendado", L"Ninguém pinta na hora: o programa pede e o Windows avisa quando pintar.", 0, -1, -1},
    {L"Etapa 2: BeginPaint → HDC", L"HDC dc = BeginPaint(janela, &pintura)", L"superfície da janela", L"O HDC é o destino dos pixels; Graphics g(dc) desenha nele.", 1, -1, -1},
    {L"Etapa 3: Graphics + pincel", L"SolidBrush(Color(105, 0, 0, 0)) → FillEllipse", L"sombra 41% opaca", L"Alfa 105 de 255: a cor se mistura com o chão que já estava lá.", 2, -1, -1},
    {L"Etapa 4: EndPaint", L"Graphics destruído → EndPaint(janela, &pintura)", L"quadro entregue", L"Sem EndPaint, a área continua \"suja\" e o WM_PAINT se repete.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: desenharMenu é feito de painel, desenharTexto e imagens, sobre fundoInterface.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 04: Formas, cores e GDI+", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 04: Formas, cores e GDI+";
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
