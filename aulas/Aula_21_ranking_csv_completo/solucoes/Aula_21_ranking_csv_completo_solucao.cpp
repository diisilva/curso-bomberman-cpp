// Aula 21 — solução do exercício 3: qual versão o .bak guarda?
//
// No jogo, gravarRanking chama CopyFileA(arquivo, arquivo.bak, TRUE). O TRUE
// significa "falhe se o destino já existir": o .bak é criado na primeira
// gravação e NUNCA mais muda. Ele guarda a versão mais antiga, não a anterior.
// Com FALSE, o .bak passa a ser sempre a versão de antes da última gravação.
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>

void gravar(const std::string& caminho, const std::string& conteudo, bool sobrescreverBackup) {
    CopyFileA(caminho.c_str(), (caminho + ".bak").c_str(), sobrescreverBackup ? FALSE : TRUE);
    std::string temporario = caminho + ".tmp";
    std::ofstream(temporario, std::ios::trunc) << conteudo << "\n";
    MoveFileExA(temporario.c_str(), caminho.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

std::string primeiraLinha(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    std::string linha;
    std::getline(arquivo, linha);
    return linha.empty() ? "(nao existe)" : linha;
}

void testar(bool sobrescreverBackup) {
    const std::string caminho = sobrescreverBackup ? "ranking_bak_anterior.csv" : "ranking_bak_jogo.csv";
    DeleteFileA(caminho.c_str());
    DeleteFileA((caminho + ".bak").c_str());
    for (const char* versao : {"versao 1", "versao 2", "versao 3"}) gravar(caminho, versao, sobrescreverBackup);
    std::cout << (sobrescreverBackup ? "CopyFileA(..., FALSE)" : "CopyFileA(..., TRUE) jogo")
              << ": arquivo=" << primeiraLinha(caminho) << "  .bak=" << primeiraLinha(caminho + ".bak") << "\n";
}

int main() {
    testar(false);   // regra do jogo: .bak congelado na versão 1
    testar(true);    // variação: .bak é sempre a versão anterior (2)
    return 0;
}
