// Aula 21: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"campoCsv", L"lerCamposCsv", L"inteiroValido", L".tmp e MoveFileEx"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: campoCsv", L"Ana; \"A\"  →  \"Ana; \"\"A\"\"\"", L"nome protegido por aspas", L"Dentro de aspas, ; é texto; uma aspa vira duas.", 0, -1, -1},
    {L"Etapa 2: lerCamposCsv", L"estado: fora ou dentro de aspas; \"\" → \"", L"5 campos: Ana; \"A\" volta igual", L"O leitor desfaz exatamente o que o escritor fez.", 1, -1, -1},
    {L"Etapa 3: inteiroValido", L"stoi(\"novecentos\") lança; \"-5\" < 0; 4 campos", L"4 linhas ignoradas, jogo segue", L"Uma linha ruim não pode derrubar o ranking inteiro.", 2, -1, -1},
    {L"Etapa 4: .tmp e MoveFileEx", L"grava ranking.csv.tmp → MoveFileExA(REPLACE)", L"nunca fica pela metade", L"Se faltar disco no meio, o arquivo antigo continua intacto.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: nomes digitados podem ter ; e aspas, e o arquivo pode ter sido editado à mão.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 21: Ranking CSV completo", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 21: Ranking CSV completo";
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
