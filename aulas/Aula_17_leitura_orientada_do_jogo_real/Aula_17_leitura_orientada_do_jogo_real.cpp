// Aula 17 — Leitura guiada do jogo real
//
// Um "índice" do bomberman.cpp feito em C++: lê o arquivo, encontra cada
// função (linha de início, linha de fim, tamanho) e diz quais outras funções
// do jogo ela chama, na ordem em que aparecem. Use-o como mapa durante a
// leitura guiada do roteiro.
//
// Uso:  Aula_17_exemplo.exe                 → roteiro principal do jogo
//       Aula_17_exemplo.exe atualizar       → só a função pedida
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

struct Funcao { std::string nome; int inicio = 0, fim = 0; };

std::vector<std::string> lerLinhas() {
    // Funciona rodando da raiz do curso, de build/ ou da pasta da aula.
    for (const char* caminho : {"projeto_final/bomberman.cpp", "../projeto_final/bomberman.cpp",
                                "../../projeto_final/bomberman.cpp"}) {
        std::ifstream arquivo(caminho);
        if (!arquivo) continue;
        std::vector<std::string> linhas;
        for (std::string linha; std::getline(arquivo, linha);) linhas.push_back(linha);
        return linhas;
    }
    return {};
}

// Conta chaves ignorando o que está dentro de "..." e '...' e depois de //.
int saldoDeChaves(const std::string& linha) {
    int saldo = 0;
    char aspa = 0;
    for (size_t i = 0; i < linha.size(); ++i) {
        char c = linha[i];
        if (aspa) { if (c == '\\') ++i; else if (c == aspa) aspa = 0; continue; }
        if (c == '"' || c == '\'') aspa = c;
        else if (c == '/' && i + 1 < linha.size() && linha[i + 1] == '/') break;
        else if (c == '{') ++saldo;
        else if (c == '}') --saldo;
    }
    return saldo;
}

// Uma definição de função começa na coluna 0: "tipo nome(parâmetros) {".
std::vector<Funcao> encontrarFuncoes(const std::vector<std::string>& linhas) {
    const std::regex definicao(R"(^(?:[A-Za-z_][\w:<>,\*&]*\s+)+[\*&]?(\w+)\s*\()");
    const std::set<std::string> palavrasChave = {"if", "for", "while", "switch", "return", "else"};
    std::vector<Funcao> funcoes;
    for (size_t i = 0; i < linhas.size(); ++i) {
        std::smatch m;
        if (!std::regex_search(linhas[i], m, definicao) || palavrasChave.count(m[1])) continue;
        // "int f();" é só uma declaração; "int f() { return 1; }" é definição de uma linha.
        size_t pontoVirgula = linhas[i].find(';'), chave = linhas[i].find('{');
        if (pontoVirgula != std::string::npos && (chave == std::string::npos || chave > pontoVirgula)) continue;
        // Procura a "{" que abre o corpo (pode estar na linha seguinte).
        size_t j = i;
        while (j < linhas.size() && linhas[j].find('{') == std::string::npos && linhas[j].find(';') == std::string::npos) ++j;
        if (j >= linhas.size() || linhas[j].find('{') == std::string::npos) continue;
        int profundidade = 0;
        size_t fim = j;
        for (; fim < linhas.size(); ++fim) {
            profundidade += saldoDeChaves(linhas[fim]);
            if (profundidade <= 0) break;
        }
        funcoes.push_back({m[1], int(i) + 1, int(fim) + 1});
        i = fim;   // não procura funções dentro desta
    }
    return funcoes;
}

// Funções do jogo chamadas no corpo, na ordem da primeira aparição.
std::vector<std::string> chamadas(const Funcao& f, const std::vector<std::string>& linhas, const std::set<std::string>& nomes) {
    std::vector<std::string> ordem;
    std::set<std::string> vistas;
    const std::regex chamada(R"(\b([A-Za-z_]\w*)\s*\()");
    for (int i = f.inicio; i < f.fim; ++i) {   // pula a linha da própria definição
        const std::string& linha = linhas[i];
        for (auto it = std::sregex_iterator(linha.begin(), linha.end(), chamada); it != std::sregex_iterator(); ++it) {
            std::string nome = (*it)[1];
            if (nome != f.nome && nomes.count(nome) && vistas.insert(nome).second) ordem.push_back(nome);
        }
    }
    return ordem;
}

void mostrar(const Funcao& f, const std::vector<std::string>& linhas, const std::set<std::string>& nomes) {
    std::cout << f.nome << "  (linhas " << f.inicio << "-" << f.fim << ", " << f.fim - f.inicio + 1 << " linhas)\n   chama:";
    auto lista = chamadas(f, linhas, nomes);
    if (lista.empty()) std::cout << " (nenhuma funcao do jogo)";
    for (const auto& n : lista) std::cout << ' ' << n;
    std::cout << "\n";
}

int main(int argc, char** argv) {
    std::vector<std::string> linhas = lerLinhas();
    if (linhas.empty()) { std::cout << "nao encontrei projeto_final/bomberman.cpp\n"; return 1; }
    std::vector<Funcao> funcoes = encontrarFuncoes(linhas);
    std::set<std::string> nomes;
    std::map<std::string, Funcao> porNome;
    for (const auto& f : funcoes) { nomes.insert(f.nome); porNome.emplace(f.nome, f); }

    if (argc > 1) {
        auto it = porNome.find(argv[1]);
        if (it == porNome.end()) { std::cout << "funcao nao encontrada: " << argv[1] << "\n"; return 2; }
        mostrar(it->second, linhas, nomes);
        return 0;
    }

    std::cout << linhas.size() << " linhas, " << funcoes.size() << " funcoes\n";
    std::vector<Funcao> maiores = funcoes;
    std::sort(maiores.begin(), maiores.end(), [](const Funcao& a, const Funcao& b) { return a.fim - a.inicio > b.fim - b.inicio; });
    std::cout << "as 5 maiores:";
    for (int i = 0; i < 5 && i < int(maiores.size()); ++i) std::cout << ' ' << maiores[i].nome << '(' << maiores[i].fim - maiores[i].inicio + 1 << ')';
    std::cout << "\n\nroteiro da leitura guiada:\n";
    for (const char* nome : {"WinMain", "processarMensagem", "atualizar", "desenhar", "novaPartida", "montarFase"})
        if (porNome.count(nome)) mostrar(porNome[nome], linhas, nomes);
    return 0;
}
