// Aula 13 — Ranking em CSV (versão simples)
//
// O ranking do jogo é um arquivo de texto: uma linha por jogador, cinco campos
// separados por ponto e vírgula (data;jogador;pontos;bombas;movimentos).
// RegistroPontuacao e melhorRegistro são os do jogo. A leitura aqui é a mais
// simples possível; a Aula 21 trata aspas, linhas inválidas e gravação segura.
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct RegistroPontuacao {
    std::string data;
    std::string jogador;
    int pontuacao = 0;
    int bombas = 0;
    int movimentos = 0;
};

// Desempate do jogo: mais pontos; depois MENOS bombas; depois MENOS passos; depois o nome.
bool melhorRegistro(const RegistroPontuacao& a, const RegistroPontuacao& b) {
    if (a.pontuacao != b.pontuacao) return a.pontuacao > b.pontuacao;
    if (a.bombas != b.bombas) return a.bombas < b.bombas;
    if (a.movimentos != b.movimentos) return a.movimentos < b.movimentos;
    return a.jogador < b.jogador;   // o jogo compara sem diferenciar maiúsculas (Aula 21)
}

bool gravarRanking(std::vector<RegistroPontuacao> registros, const std::string& caminho) {
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    std::ofstream arquivo(caminho, std::ios::trunc);
    if (!arquivo) return false;
    for (const auto& r : registros)
        arquivo << r.data << ';' << r.jogador << ';' << r.pontuacao << ';' << r.bombas << ';' << r.movimentos << '\n';
    return bool(arquivo);
}

std::vector<RegistroPontuacao> lerRanking(const std::string& caminho) {
    std::vector<RegistroPontuacao> registros;
    std::ifstream arquivo(caminho);
    std::string linha;
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue;
        std::stringstream partes(linha);
        std::string campos[5];
        for (std::string& campo : campos) std::getline(partes, campo, ';');   // corta em cada ';'
        RegistroPontuacao r;
        r.data = campos[0];
        r.jogador = campos[1];
        r.pontuacao = std::stoi(campos[2]);    // a Aula 21 trata texto inválido aqui
        r.bombas = std::stoi(campos[3]);
        r.movimentos = std::stoi(campos[4]);
        registros.push_back(r);
    }
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    return registros;
}

int main() {
    const std::string caminho = "ranking_aula.csv";
    std::vector<RegistroPontuacao> partidas = {
        {"13/10/2026", "Caio", 700, 2, 30},
        {"13/10/2026", "Ana", 900, 3, 40},
        {"14/10/2026", "Bia", 900, 3, 35},    // empata com Ana em pontos e bombas
        {"14/10/2026", "Davi", 900, 2, 50},   // mesmos pontos, menos bombas
    };
    if (!gravarRanking(partidas, caminho)) { std::cout << "nao consegui gravar\n"; return 1; }

    std::cout << "conteudo de " << caminho << ":\n";
    std::ifstream bruto(caminho);
    for (std::string linha; std::getline(bruto, linha);) std::cout << "  " << linha << "\n";

    std::cout << "ranking relido:\n";
    int posicao = 1;
    for (const auto& r : lerRanking(caminho))
        std::cout << "  " << posicao++ << ". " << r.jogador << " " << r.pontuacao << " pts, "
                  << r.bombas << " bombas, " << r.movimentos << " passos\n";
    return 0;
}
