// Aula 21 — Ranking CSV completo
//
// As funções de arquivo do jogo real (projeto_final/bomberman.cpp), quase sem mudança:
//   campoCsv          → protege um campo com aspas quando ele tem ; " ou quebra de linha
//   lerCamposCsv      → o inverso: separa os 5 campos respeitando as aspas
//   lerRanking        → valida cada linha e PULA as quebradas, sem derrubar o jogo
//   gravarRanking     → grava num .tmp e só então troca pelo arquivo verdadeiro
#include <windows.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct RegistroPontuacao { std::string data, jogador; int pontuacao = 0, bombas = 0, movimentos = 0; };

std::string campoCsv(const std::string& campo) {
    if (campo.find_first_of(";\"\r\n") == std::string::npos) return campo;
    std::string saida = "\"";
    for (char c : campo) { if (c == '\"') saida += '\"'; saida += c; }   // " vira ""
    return saida + '\"';
}

// Uma máquina de estados de dois estados: dentro ou fora de aspas.
bool lerCamposCsv(const std::string& linha, std::vector<std::string>& campos) {
    campos.clear();
    std::string campo;
    bool aspas = false, fechado = false;
    for (size_t i = 0; i < linha.size(); ++i) {
        char c = linha[i];
        if (aspas) {
            if (c == '\"') {
                if (i + 1 < linha.size() && linha[i + 1] == '\"') { campo += '\"'; ++i; }   // "" → "
                else { aspas = false; fechado = true; }
            } else campo += c;                                    // ; dentro de aspas é texto
        } else if (c == ';') { campos.push_back(campo); campo.clear(); fechado = false; }
        else if (c == '\"') { if (!campo.empty() || fechado) return false; aspas = true; }
        else if (c != '\r') { if (fechado) return false; campo += c; }
    }
    if (aspas) return false;                                      // aspa aberta e nunca fechada
    campos.push_back(campo);
    return campos.size() == 5;
}

bool melhorRegistro(const RegistroPontuacao& a, const RegistroPontuacao& b) {
    if (a.pontuacao != b.pontuacao) return a.pontuacao > b.pontuacao;
    if (a.bombas != b.bombas) return a.bombas < b.bombas;
    if (a.movimentos != b.movimentos) return a.movimentos < b.movimentos;
    return a.jogador < b.jogador;
}

int linhasIgnoradas = 0;   // só para o relatório; o jogo simplesmente pula
std::vector<RegistroPontuacao> lerRanking(const std::string& caminho) {
    std::vector<RegistroPontuacao> registros;
    std::ifstream arquivo(caminho);
    std::string linha;
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue;
        std::vector<std::string> campos;
        if (!lerCamposCsv(linha, campos)) { linhasIgnoradas++; continue; }
        RegistroPontuacao r;
        r.data = campos[0];
        r.jogador = campos[1];
        // Mesma lambda do jogo: o texto INTEIRO precisa ser um número >= 0.
        auto inteiroValido = [](const std::string& valor, int& destino) {
            try { size_t fim; destino = std::stoi(valor, &fim); return fim == valor.size() && destino >= 0; }
            catch (...) { return false; }
        };
        if (!inteiroValido(campos[2], r.pontuacao) || !inteiroValido(campos[3], r.bombas) ||
            !inteiroValido(campos[4], r.movimentos)) { linhasIgnoradas++; continue; }
        registros.push_back(r);
    }
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    return registros;
}

bool gravarRanking(std::vector<RegistroPontuacao> registros, const std::string& caminhoArquivo) {
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    // TRUE = não sobrescrever: o .bak só é criado se ainda não existir.
    CopyFileA(caminhoArquivo.c_str(), (caminhoArquivo + ".bak").c_str(), TRUE);
    std::string temporario = caminhoArquivo + ".tmp";
    std::ofstream arquivo(temporario, std::ios::trunc);
    if (!arquivo) return false;
    for (const auto& r : registros)
        arquivo << campoCsv(r.data) << ';' << campoCsv(r.jogador) << ';' << r.pontuacao << ';' << r.bombas << ';' << r.movimentos << '\n';
    arquivo.flush();
    if (!arquivo) return false;   // disco cheio? o arquivo verdadeiro continua intacto
    arquivo.close();
    return MoveFileExA(temporario.c_str(), caminhoArquivo.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
}

int main() {
    const std::string caminho = "ranking_avancado.csv";
    DeleteFileA(caminho.c_str());
    DeleteFileA((caminho + ".bak").c_str());

    gravarRanking({{"13/10/2026", "Ana; \"A\"", 900, 4, 24}, {"13/10/2026", "Bia", 800, 2, 20}}, caminho);
    std::cout << "gravado:\n";
    std::ifstream bruto(caminho);
    for (std::string linha; std::getline(bruto, linha);) std::cout << "  " << linha << "\n";
    bruto.close();

    // Linhas estragadas, como se alguém tivesse editado o arquivo à mão.
    std::ofstream(caminho, std::ios::app) << "14/10/2026;Caio;novecentos;1;1\n"   // não é número
                                          << "14/10/2026;Davi;-5;1;1\n"            // negativo
                                          << "14/10/2026;Eva;500;1\n"              // 4 campos
                                          << "14/10/2026;\"Fabio;300;1;1\n";       // aspa sem fechar
    std::cout << "relido:\n";
    for (const auto& r : lerRanking(caminho)) std::cout << "  " << r.jogador << " pontos=" << r.pontuacao << "\n";
    std::cout << "linhas ignoradas=" << linhasIgnoradas << "\n";
    return 0;
}
