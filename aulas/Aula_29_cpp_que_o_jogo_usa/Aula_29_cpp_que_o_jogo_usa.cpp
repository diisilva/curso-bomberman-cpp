// Aula 29 — O C++ que o jogo usa
//
// Oito recursos da linguagem que aparecem no bomberman.cpp e que costumam
// ficar de fora de um primeiro curso de C++. Cada bloco imita um uso real do
// jogo; o roteiro mostra o trecho original ao lado.
#include <windows.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// 1. struct com valores padrão (como Bomba e EntradaControle)
struct EntradaControle { int dl = 0, dc = 0; bool bomba = false; };

// 2. enum + switch (como TipoItem em coletarItensEm)
enum TipoItem { CHUTE, TELEPORTE, RAIO };
const char* nomeDoItem(TipoItem t) {
    switch (t) {
        case CHUTE: return "chute";
        case TELEPORTE: return "teleporte";
        case RAIO: return "raio";
    }
    return "?";
}

// 3. referência constante: lê sem copiar e sem poder alterar (como melhorRegistro)
struct Registro { std::string jogador; int pontuacao; };
bool melhor(const Registro& a, const Registro& b) { return a.pontuacao > b.pontuacao; }

// 4. template: uma função, vários tipos de critério (como bfsProximoPasso)
template <typename Criterio>
int contarSe(const std::vector<int>& valores, Criterio criterio) {
    int total = 0;
    for (int v : valores) if (criterio(v)) ++total;
    return total;
}

// 5. unique_ptr: o objeto é destruído sozinho quando o dono some (como carregarSprite)
struct Imagem {
    std::string nome;
    explicit Imagem(std::string n) : nome(std::move(n)) { std::printf("  carregou %s\n", nome.c_str()); }
    ~Imagem() { std::printf("  liberou %s\n", nome.c_str()); }
};
std::unique_ptr<Imagem> carregarSprite(const std::string& nome) { return std::make_unique<Imagem>(nome); }

// 6. ponteiro de função (como FuncaoXInputGetState, carregada de uma DLL)
using Operacao = int (*)(int, int);
int somar(int a, int b) { return a + b; }

// 7. wstring ↔ UTF-8 (como wparaStr: a tela usa wstring, o ranking grava UTF-8)
std::string wparaStr(const std::wstring& texto) {
    int n = WideCharToMultiByte(CP_UTF8, 0, texto.data(), int(texto.size()), nullptr, 0, nullptr, nullptr);
    std::string resultado(n, '\0');
    if (n) WideCharToMultiByte(CP_UTF8, 0, texto.data(), int(texto.size()), resultado.data(), n, nullptr, nullptr);
    return resultado;
}

int main() {
    std::printf("1. struct com valores padrao\n");
    EntradaControle e;                     // nada foi escrito, mas tudo começa zerado
    std::printf("  dl=%d dc=%d bomba=%d\n", e.dl, e.dc, e.bomba);

    std::printf("2. enum + switch\n");
    std::printf("  item 2 = %s\n", nomeDoItem(RAIO));

    std::printf("3. const& + sort com comparador\n");
    std::vector<Registro> ranking = {{"Ana", 700}, {"Bia", 900}, {"Caio", 800}};
    std::sort(ranking.begin(), ranking.end(), melhor);
    std::printf("  1o lugar: %s\n", ranking[0].jogador.c_str());

    std::printf("4. lambda + template\n");
    std::vector<int> vidas = {55, 74, 92, 115, 129};
    int limite = 100;
    int fortes = contarSe(vidas, [limite](int v) { return v > limite; });   // [limite] copia a variável
    int pares = contarSe(vidas, [](int v) { return v % 2 == 0; });
    std::printf("  chefes com vida > %d: %d; com vida par: %d\n", limite, fortes, pares);

    std::printf("5. vector + erase/remove_if (como os projeteis mortos)\n");
    std::vector<int> projeteis = {3, 0, 2, 0, 1};    // 0 = sem vida
    projeteis.erase(std::remove_if(projeteis.begin(), projeteis.end(), [](int vida) { return vida <= 0; }), projeteis.end());
    std::printf("  sobraram %zu projeteis\n", projeteis.size());

    std::printf("6. unique_ptr\n");
    {
        std::unique_ptr<Imagem> jogador = carregarSprite("jogador.png");
        std::printf("  usando %s\n", jogador->nome.c_str());
    }                                     // aqui o unique_ptr sai de escopo e libera a imagem
    std::printf("  (fim do bloco)\n");

    std::printf("7. ponteiro de funcao\n");
    Operacao op = somar;
    std::printf("  op(2, 3) = %d\n", op(2, 3));

    std::printf("8. wstring -> UTF-8\n");
    std::string utf8 = wparaStr(L"João");
    std::printf("  \"Jo\\u00e3o\" tem 4 letras e %zu bytes em UTF-8\n", utf8.size());
    return 0;
}
