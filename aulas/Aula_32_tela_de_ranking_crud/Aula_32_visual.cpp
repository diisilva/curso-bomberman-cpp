// Aula 32: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"chave do nome", L"Read e página", L"Update", L"Delete confirmado"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: chave do nome", L"chaveNomeRanking(\"  ANA \") == chaveNomeRanking(\"ana\")", L"a mesma pessoa, um registro só", L"Maiúsculas e espaços extras não podem criar um segundo jogador.", 0, -1, -1},
    {L"Etapa 2: Read e página", L"inicio = selecionado / 10 * 10", L"registro 11 → página 2", L"A página sai da seleção; não existe uma variável \"página\" para ficar errada.", 1, -1, -1},
    {L"Etapa 3: Update", L"editarNomeRanking(\"Caio\", \" BIA \") → false", L"700 pontos preservados; nome repetido recusado", L"Renomear troca só o nome e nunca funde dois jogadores.", 2, -1, -1},
    {L"Etapa 4: Delete confirmado", L"DEL → pergunta → S → excluirRegistroRanking", L"nome livre para recomeçar do zero", L"Apagar é irreversível: a tela pergunta antes e mostra o que se perde.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: a opção 6 do menu relê ranking.csv a cada desenho; E renomeia e DEL exclui o nome selecionado.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 32: A tela de ranking: listar, renomear e excluir", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 32: A tela de ranking: listar, renomear e excluir";
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
