// Aula 29: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"struct + padrão", L"lambda", L"template", L"unique_ptr"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: struct + padrão", L"struct EntradaControle { int dl = 0, dc = 0; bool bomba = false; };", L"EntradaControle e; → tudo zerado", L"Valores padrão evitam variáveis com lixo de memória.", 0, -1, -1},
    {L"Etapa 2: lambda", L"[limite](int v) { return v > limite; }", L"2 chefes com vida > 100", L"Uma função curta, criada onde é usada, que pode guardar variáveis.", 1, -1, -1},
    {L"Etapa 3: template", L"template <typename Criterio> int contarSe(..., Criterio c)", L"a mesma função aceita qualquer lambda", L"Cada lambda tem um tipo próprio; o template aceita todos.", 2, -1, -1},
    {L"Etapa 4: unique_ptr", L"{ auto jogador = carregarSprite(\"jogador.png\"); }", L"liberou jogador.png ao sair do bloco", L"Quem é dono do recurso o libera sozinho: sem delete esquecido.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: struct, enum, vector, lambda, template e unique_ptr aparecem em quase toda função.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 29: O C++ que o jogo usa", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 29: O C++ que o jogo usa";
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
