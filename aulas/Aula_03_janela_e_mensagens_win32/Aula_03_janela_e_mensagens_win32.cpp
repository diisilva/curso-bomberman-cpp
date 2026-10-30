// Aula 03 — Janela e mensagens Win32
//
// Um esqueleto com a mesma forma de WinMain e processarMensagem do jogo:
// registrar a classe, criar a janela, girar o laço de mensagens e tratar
// cada mensagem num switch. O título da janela mostra as mensagens chegando.
#include <windows.h>
#include <string>

const int PASSO_LOGICA_MS = 16;   // mesmo intervalo pedido pelo jogo
int ticks = 0;                    // quantos WM_TIMER já chegaram
ULONGLONG inicioMs = 0;           // relógio real, para comparar com o pedido
std::wstring ultimaTecla = L"nenhuma";

void atualizarTitulo(HWND janela) {
    ULONGLONG passouMs = GetTickCount64() - inicioMs;
    int porSegundo = passouMs ? int(ticks * 1000 / passouMs) : 0;
    // Pedimos um tick a cada 16 ms (62 por segundo). Compare com o que chega.
    std::wstring titulo = L"WM_TIMER: " + std::to_wstring(ticks) + L" (" + std::to_wstring(porSegundo) +
                          L" por segundo)   ultima tecla: " + ultimaTecla;
    SetWindowTextW(janela, titulo.c_str());
}

// O Windows chama esta função para cada mensagem da janela. No jogo ela se
// chama processarMensagem e tem um case para cada evento que importa.
LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_CREATE:
        // A janela existe, mas ainda não apareceu: bom lugar para preparar o jogo.
        SetTimer(janela, 1, PASSO_LOGICA_MS, nullptr);
        inicioMs = GetTickCount64();
        return 0;
    case WM_TIMER:
        // No jogo, é aqui que o estado avança (Aula 06). Atualizar o título a
        // cada tick faria a barra piscar; uma vez a cada 30 ticks basta.
        if (++ticks % 30 == 0) atualizarTitulo(janela);
        return 0;
    case WM_KEYDOWN:
        // wParam traz o código virtual da tecla: 'A'..'Z', VK_SPACE, VK_LEFT...
        if (wParam == VK_ESCAPE) { DestroyWindow(janela); return 0; }
        if ((wParam >= 'A' && wParam <= 'Z') || (wParam >= '0' && wParam <= '9'))
            ultimaTecla = std::wstring(1, static_cast<wchar_t>(wParam));
        else
            ultimaTecla = L"codigo " + std::to_wstring(wParam);   // setas: 37 a 40; espaço: 32
        atualizarTitulo(janela);
        return 0;
    case WM_DESTROY:
        // Ordem do jogo: parar o timer, liberar recursos, pedir o fim do laço.
        KillTimer(janela, 1);
        PostQuitMessage(0);
        return 0;
    }
    // Tudo o que não tratamos (mover, redimensionar, fechar...) fica com o padrão.
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;   // liga a classe à função acima
    classe.hInstance = instancia;
    classe.lpszClassName = L"AulaJanela";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    classe.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassW(&classe)) return 1;

    // Queremos 640 x 480 de ÁREA ÚTIL; AdjustWindowRect soma bordas e título.
    RECT area{0, 0, 640, 480};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 03", WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, area.right - area.left, area.bottom - area.top,
                                  nullptr, nullptr, instancia, nullptr);
    if (!janela) return 2;
    ShowWindow(janela, exibir);
    UpdateWindow(janela);

    // O laço de mensagens: GetMessage espera o próximo evento e só devolve 0
    // depois de PostQuitMessage. Enquanto isso, o programa fica vivo aqui.
    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) {
        TranslateMessage(&evento);   // transforma teclas em WM_CHAR (usado nos nomes, Aula 30)
        DispatchMessageW(&evento);   // entrega a mensagem a processarMensagem
    }
    return static_cast<int>(evento.wParam);
}
