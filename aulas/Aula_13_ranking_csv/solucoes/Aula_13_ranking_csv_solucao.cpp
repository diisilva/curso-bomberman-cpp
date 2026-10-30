// Aula 13 — solução do exercício 2: um registro por jogador, sempre o melhor.
//
// salvarRegistroPartida, no jogo, faz três passos: lê o ranking, procura o
// mesmo jogador (sem diferenciar maiúsculas) e só substitui se a partida
// nova for melhor; depois grava tudo. Aqui a chave do nome é um tolower
// simples (o jogo usa LCMapStringEx para funcionar também com acentos).
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct RegistroPontuacao { std::string data, jogador; int pontuacao = 0, bombas = 0, movimentos = 0; };

std::string chaveNomeRanking(const std::string& nome) {
    std::string chave = nome;
    for (char& c : chave) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return chave;
}

bool melhorRegistro(const RegistroPontuacao& a, const RegistroPontuacao& b) {
    if (a.pontuacao != b.pontuacao) return a.pontuacao > b.pontuacao;
    if (a.bombas != b.bombas) return a.bombas < b.bombas;
    if (a.movimentos != b.movimentos) return a.movimentos < b.movimentos;
    return chaveNomeRanking(a.jogador) < chaveNomeRanking(b.jogador);
}

std::vector<RegistroPontuacao> lerRanking(const std::string& caminho) {
    std::vector<RegistroPontuacao> registros;
    std::ifstream arquivo(caminho);
    for (std::string linha; std::getline(arquivo, linha);) {
        if (linha.empty()) continue;
        std::stringstream partes(linha);
        std::string c[5];
        for (std::string& campo : c) std::getline(partes, campo, ';');
        registros.push_back({c[0], c[1], std::stoi(c[2]), std::stoi(c[3]), std::stoi(c[4])});
    }
    return registros;
}

bool gravarRanking(std::vector<RegistroPontuacao> registros, const std::string& caminho) {
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    std::ofstream arquivo(caminho, std::ios::trunc);
    for (const auto& r : registros)
        arquivo << r.data << ';' << r.jogador << ';' << r.pontuacao << ';' << r.bombas << ';' << r.movimentos << '\n';
    return bool(arquivo);
}

bool salvarRegistroPartida(const RegistroPontuacao& novo, const std::string& caminho) {
    auto registros = lerRanking(caminho);
    auto existente = std::find_if(registros.begin(), registros.end(), [&](const RegistroPontuacao& r) {
        return chaveNomeRanking(r.jogador) == chaveNomeRanking(novo.jogador);
    });
    if (existente == registros.end()) registros.push_back(novo);   // jogador novo
    else if (melhorRegistro(novo, *existente)) *existente = novo;   // recorde pessoal
    return gravarRanking(registros, caminho);                      // senão, nada muda
}

int main() {
    const std::string caminho = "ranking_solucao13.csv";
    std::ofstream(caminho, std::ios::trunc).close();   // começa vazio
    salvarRegistroPartida({"13/10/2026", "Ana", 900, 3, 40}, caminho);
    salvarRegistroPartida({"13/10/2026", "Bia", 800, 2, 30}, caminho);
    salvarRegistroPartida({"14/10/2026", "Ana", 600, 1, 20}, caminho);   // pior: ignorada
    salvarRegistroPartida({"15/10/2026", "ana", 950, 4, 60}, caminho);   // mesmo jogador, melhor
    for (const auto& r : lerRanking(caminho))
        std::cout << r.jogador << " " << r.pontuacao << " (" << r.data << ")\n";
    return 0;
}
