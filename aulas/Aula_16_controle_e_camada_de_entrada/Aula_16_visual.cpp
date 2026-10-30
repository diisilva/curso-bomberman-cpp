// Aula 16: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"XINPUT_GAMEPAD", L"direcional primeiro", L"zona morta", L"EntradaControle"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: XINPUT_GAMEPAD", L"wButtons = DPAD_UP | A; sThumbLX = 0", L"bits de botões + eixos −32768..32767", L"Cada API entrega o controle num formato diferente.", 0, -1, -1},
    {L"Etapa 2: direcional primeiro", L"if (cima) dl = −1; else ... else analógico", L"dl = −1: cima vence", L"O direcional é preciso; o analógico só vale se ele estiver solto.", 1, -1, -1},
    {L"Etapa 3: zona morta", L"|5000| < 10500 → parado; (15000, 20000) → eixo Y", L"sem tremida, sem diagonal", L"Analógico parado nunca marca zero exato; o jogo tem grade de 4 direções.", 2, -1, -1},
    {L"Etapa 4: EntradaControle", L"A ou B → bomba; X chute; Y teleporte; ombros detonar", L"{dl, dc, bomba, ...}", L"As regras leem intenções, não botões de um aparelho.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: SDL2, XInput e DirectInput viram uma só EntradaControle antes de chegar às regras.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 16: Controle e camada de entrada", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 16: Controle e camada de entrada";
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
