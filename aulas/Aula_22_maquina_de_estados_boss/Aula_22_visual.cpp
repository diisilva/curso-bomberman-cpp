// Aula 22: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"H_PREP_AGULHA", L"H_LANCAR + voo", L"dano no meio", L"H_RECOLHER → IDLE"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: H_PREP_AGULHA", L"entrarEtapaHornet(H_PREP_AGULHA, 750)", L"aviso de 750 ms", L"Toda etapa tem duração; a preparação dá tempo de reagir.", 0, -1, -1},
    {L"Etapa 2: H_LANCAR + voo", L"tempo = 0 → LANCAR (360) + soltarAgulhaHornet()", L"agulha no ar", L"Entrar numa etapa pode disparar um efeito, só uma vez.", 1, -1, -1},
    {L"Etapa 3: dano no meio", L"reagirDanoHornet → H_DANO (480) → H_RECUP_DANO (420)", L"volta ao voo: a agulha ainda não voltou", L"A próxima etapa depende do mundo, não só da etapa atual.", 2, -1, -1},
    {L"Etapa 4: H_RECOLHER → IDLE", L"agulha voltou → RECOLHER (480) → IDLE", L"t = 3248: pronta de novo", L"Um enum nomeado evita combinações impossíveis de bools.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: a Hornet tem 22 etapas (enum EtapaHornet); um switch escolhe a próxima.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 22: Máquina de estados do chefe", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 22: Máquina de estados do chefe";
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
