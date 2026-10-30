// Aula 03: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {L"RegisterClassW", L"CreateWindowExW", L"laço GetMessage", L"WM_DESTROY"};
const visual::Etapa etapas[4] = {
    {L"Etapa 1: RegisterClassW", L"classe.lpfnWndProc = processarMensagem", L"o Windows sabe quem chamar", L"A classe liga cada janela dela a uma função de mensagens.", 0, -1, -1},
    {L"Etapa 2: CreateWindowExW", L"AdjustWindowRect(área útil) → CreateWindowExW", L"HWND: a janela existe", L"AdjustWindowRect soma bordas e título ao tamanho útil pedido.", 1, -1, -1},
    {L"Etapa 3: laço GetMessage", L"while (GetMessageW(...) > 0) DispatchMessageW", L"cada evento → processarMensagem", L"O programa passa quase toda a vida esperando neste laço.", 2, -1, -1},
    {L"Etapa 4: WM_DESTROY", L"KillTimer; PostQuitMessage(0)", L"GetMessage devolve 0: fim", L"Sem PostQuitMessage a janela some, mas o processo fica vivo.", 3, -1, -1}
};
void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::Fluxo, itens, etapas, nullptr, 0, 0, nullptr, L"No jogo: WinMain cria a janela; processarMensagem trata timer, teclado, pintura e fim.");

}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    return visual::executar(instancia, exibir, L"Aula 03: Janela e mensagens Win32", desenharAula);
}
#else
int main() {
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo=L"Aula 03: Janela e mensagens Win32";
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
