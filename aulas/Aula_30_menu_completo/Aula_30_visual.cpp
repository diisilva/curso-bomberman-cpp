// Aula 30: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"estado explícito", L"WM_CHAR digita", L"mouse com escala", L"joystick = teclado"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: estado explícito", L"enum TelaMenu telaMenu; TelaMenu telaAposNomes", L"uma tela por vez; sabe para onde voltar", L"O mesmo campo de nome serve à rota Jogar e ao atalho das fases.", 0, -1, -1},
    {L"Etapa 2: WM_CHAR digita", L"TranslateMessage gera WM_CHAR; WM_KEYDOWN move o cursor", L"\"Joao\" vira \"João\"", L"WM_CHAR já chega com acento e maiúscula resolvidos.", 1, -1, -1},
    {L"Etapa 3: mouse com escala", L"x_menu = (x_janela − margemX) / escala", L"clique certo em qualquer tamanho", L"O desenho é escalado; o clique precisa da conta inversa.", 2, -1, -1},
    {L"Etapa 4: joystick = teclado", L"toque em A → lerTeclado(VK_RETURN, 0)", L"nenhuma regra nova para o controle", L"Uma tela nova funciona no controle sem código extra.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: onze telas, um campo de texto, mouse e joystick passando pelo mesmo lerTeclado.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 30: Construindo um menu completo", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 30: Construindo um menu completo";
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
