// Aula 17 — solução do exercício 3: quem chama uma função?
//
// O exemplo responde "o que esta função chama?". A pergunta inversa é tão
// útil quanto: antes de mudar montarFase, você quer saber quem depende dela.
// A solução reaproveita o exemplo inteiro: o main dele é renomeado e este
// arquivo define outro.
#define main main_do_exemplo
#include "../Aula_17_leitura_orientada_do_jogo_real.cpp"
#undef main

int main(int argc, char** argv) {
    std::string alvo = argc > 1 ? argv[1] : "montarFase";
    std::vector<std::string> linhas = lerLinhas();
    if (linhas.empty()) { std::cout << "nao encontrei projeto_final/bomberman.cpp\n"; return 1; }
    std::vector<Funcao> funcoes = encontrarFuncoes(linhas);
    std::set<std::string> nomes;
    for (const auto& f : funcoes) nomes.insert(f.nome);

    std::cout << "quem chama " << alvo << ":";
    int total = 0;
    for (const auto& f : funcoes) {
        auto lista = chamadas(f, linhas, nomes);
        if (std::find(lista.begin(), lista.end(), alvo) != lista.end()) {
            std::cout << ' ' << f.nome << '(' << f.inicio << ')';
            ++total;
        }
    }
    std::cout << "\n" << total << " funcoes\n";
    return 0;
}
