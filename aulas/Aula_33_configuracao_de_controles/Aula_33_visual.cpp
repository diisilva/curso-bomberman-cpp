// Aula 33: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"escolher × medir", L"ladoDispositivo", L"validarAtribuicao", L"escolha → teclas"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: escolher × medir", L"doisJogadores, dispositivoP1/P2 escolhidos; controleConectado medido", L"plugar o controle não cria jogador", L"Só a tela de quantidade decide quantos jogam.", 0, -1, -1},
    {L"Etapa 2: ladoDispositivo", L"ladoDispositivo: −1 = P1, 0 = SEM USO, 1 = P2", L"o cartão vai para a coluna certa", L"Três colunas viram um número; o desenho só lê esse número.", 1, -1, -1},
    {L"Etapa 3: validarAtribuicao", L"2P e P1 == P2 → P2 recebe o outro aparelho", L"nunca dois no mesmo joystick", L"A regra é conferida ao abrir a tela e ao confirmar.", 2, -1, -1},
    {L"Etapa 4: escolha → teclas", L"teclaDoJogador: WASD = 1, SETAS = 2, ambos = 3", L"desconectou: o teclado volta ao dono", L"controleNoJogador pergunta \"está conectado?\" a cada quadro.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: JOGAR → quantos vão jogar → nomes → quem usa cada controle → personagens.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 33: Configurando os controles: quem joga com o quê", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 33: Configurando os controles: quem joga com o quê";
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
