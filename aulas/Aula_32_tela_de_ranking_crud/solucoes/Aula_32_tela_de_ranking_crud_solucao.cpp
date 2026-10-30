// Solução do exercício 3 da Aula 32: desfazer a última exclusão.
//
// A exclusão guarda uma cópia do registro antes de apagar. Desfazer usa o
// próprio salvarRegistroPartida: se o nome voltou a jogar depois de excluído,
// fica o melhor dos dois resultados, sem criar uma linha repetida.
// Programa de console: reaproveita as funções da aula (a janela não abre).
#define WinMain WinMain_da_aula
#include "../Aula_32_tela_de_ranking_crud.cpp"
#undef WinMain
#include <cstdio>

RegistroPontuacao ultimoExcluido;
bool podeDesfazer = false;

bool excluirComDesfazer(const std::string& nome) {
    for (const auto& r : lerRanking())
        if (mesmoNome(r.jogador, nome)) ultimoExcluido = r;
    podeDesfazer = excluirRegistroRanking(nome);
    return podeDesfazer;
}
bool desfazerExclusao() {
    if (!podeDesfazer) return false;   // só uma vez: desfazer duas vezes não duplica
    podeDesfazer = false;
    return salvarRegistroPartida(ultimoExcluido);
}

void mostrar(const char* titulo) {
    std::printf("%s\n", titulo);
    auto r = lerRanking();
    for (int i = 0; i < std::min(4, int(r.size())); ++i)
        std::printf("  %d. %-6s %4d pts\n", i + 1, r[i].jogador.c_str(), r[i].pontuacao);
    std::printf("  (%zu registros)\n", r.size());
}

int main() {
    caminhoRanking = "ranking_solucao32.csv";
    DeleteFileA(caminhoRanking.c_str());
    DeleteFileA((caminhoRanking + ".bak").c_str());
    criarRankingDeExemplo();
    mostrar("inicio:");

    excluirComDesfazer("bia");
    mostrar("excluiu Bia:");

    RegistroPontuacao nova{"29/10/2026", "Bia", 200, 0, 5};
    salvarRegistroPartida(nova);
    mostrar("Bia jogou de novo e fez 200:");

    std::printf("desfazer: %s\n", desfazerExclusao() ? "ok" : "nada a desfazer");
    mostrar("desfez a exclusao (fica o melhor, 900):");
    std::printf("desfazer de novo: %s\n", desfazerExclusao() ? "ok" : "nada a desfazer");

    DeleteFileA(caminhoRanking.c_str());
    DeleteFileA((caminhoRanking + ".bak").c_str());
    return 0;
}
