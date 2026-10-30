// Ligado somente aos exemplos de console distribuidos em build/internos/exemplos.
// Mantem a saida na tela quando o aluno abre o .exe por duplo clique.
#include <windows.h>

namespace {
struct PausaAoFechar {
    ~PausaAoFechar() noexcept {
        if (GetEnvironmentVariableW(L"CURSO_BOMBERMAN_SEM_PAUSA", nullptr, 0) != 0) return;

        const HANDLE entrada = GetStdHandle(STD_INPUT_HANDLE);
        const HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD modo = 0;
        if (entrada == nullptr || entrada == INVALID_HANDLE_VALUE ||
            saida == nullptr || saida == INVALID_HANDLE_VALUE ||
            !GetConsoleMode(entrada, &modo)) return;

        constexpr wchar_t mensagem[] = L"\r\nPressione Enter para fechar este exemplo...";
        DWORD escritos = 0;
        if (!WriteConsoleW(saida, mensagem,
                           static_cast<DWORD>((sizeof(mensagem) / sizeof(wchar_t)) - 1),
                           &escritos, nullptr)) return;

        wchar_t resposta[2] = {};
        DWORD lidos = 0;
        ReadConsoleW(entrada, resposta, 2, &lidos, nullptr);
    }
};

PausaAoFechar pausaAoFechar;
} // namespace
