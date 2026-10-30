// Aula 03 — solução do exercício 2: lembrar quais teclas estão SEGURADAS.
//
// WM_KEYDOWN avisa que a tecla desceu; WM_KEYUP, que subiu. O jogo guarda o
// meio-termo num vetor bool teclaPressionada[256], porque andar depende de a
// tecla continuar apertada, não de um único aperto. WM_KILLFOCUS limpa tudo:
// se o jogador troca de janela segurando uma seta, o WM_KEYUP nunca chega aqui.
#include <windows.h>
#include <string>

bool teclaPressionada[256] = {};

int quantasSeguradas() {
    int total = 0;
    for (bool apertada : teclaPressionada) total += apertada;
    return total;
}

void atualizarTitulo(HWND janela) {
    std::wstring titulo = L"teclas seguradas: " + std::to_wstring(quantasSeguradas()) +
                          (teclaPressionada[VK_RIGHT] ? L"   (andando para a direita)" : L"");
    SetWindowTextW(janela, titulo.c_str());
}

LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) { DestroyWindow(janela); return 0; }
        if (wParam < 256) teclaPressionada[wParam] = true;
        atualizarTitulo(janela);
        return 0;
    case WM_KEYUP:
        if (wParam < 256) teclaPressionada[wParam] = false;
        atualizarTitulo(janela);
        return 0;
    case WM_KILLFOCUS:
        // Igual ao jogo: sem isso, o personagem continuaria andando sozinho.
        for (bool& apertada : teclaPressionada) apertada = false;
        atualizarTitulo(janela);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;
    classe.hInstance = instancia;
    classe.lpszClassName = L"AulaJanelaTeclas";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    classe.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassW(&classe)) return 1;
    RECT area{0, 0, 640, 480};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Segure algumas teclas", WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, area.right - area.left, area.bottom - area.top,
                                  nullptr, nullptr, instancia, nullptr);
    if (!janela) return 2;
    ShowWindow(janela, exibir);
    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) {
        TranslateMessage(&evento);
        DispatchMessageW(&evento);
    }
    return static_cast<int>(evento.wParam);
}
