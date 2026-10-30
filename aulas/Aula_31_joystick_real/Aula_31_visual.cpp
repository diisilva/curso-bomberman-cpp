// Aula 31: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"SDL2 → XInput → DI", L"normalizar", L"toque (borda)", L"foco e reconexão"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: SDL2 → XInput → DI", L"SDL2.dll achou controle? senão XInput; senão DirectInput", L"uma origem por vez", L"Cada API cobre controles que as outras leem mal ou não leem.", 0, -1, -1},
    {L"Etapa 2: normalizar", L"eixo 0..65535 → −1000..1000; POV 9000 → direita", L"todo controle na mesma escala", L"Controles genéricos informam faixas e formatos diferentes.", 1, -1, -1},
    {L"Etapa 3: toque (borda)", L"novo(agora, antes) = agora && !antes", L"segurar A = 1 bomba, não 60", L"Ações de menu e bomba disparam quando o botão desce.", 2, -1, -1},
    {L"Etapa 4: foco e reconexão", L"fora de foco → leitura zerada; ao conectar, antes = agora", L"sem movimento nem bomba fantasma", L"O controle só age na janela do jogo, e só depois de solto.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: atualizarEstadoControle roda a cada WM_TIMER e escolhe uma das três APIs.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 31: Conectando um joystick de verdade", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 31: Conectando um joystick de verdade";
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
