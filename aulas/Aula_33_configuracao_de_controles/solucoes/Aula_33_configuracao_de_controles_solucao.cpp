// Solução do exercício 3 da Aula 33: lembrar a configuração entre partidas.
//
// O jogo pergunta tudo de novo a cada partida. Aqui a escolha é gravada em
// controles_aula33.cfg, um "chave=valor" por linha, e lida na próxima vez.
// O arquivo é dado de fora: qualquer valor estranho vira o padrão, e a leitura
// termina com validarAtribuicao, porque o joystick pode não estar conectado hoje.
// Programa de console: reaproveita as funções da aula (a janela não abre).
#define WinMain WinMain_da_aula
#include "../Aula_33_configuracao_de_controles.cpp"
#undef WinMain
#include <cstdio>
#include <fstream>

const char* nomeDispositivo(Dispositivo d) { return d == TECLADO ? "teclado" : "joystick"; }

bool salvarConfiguracao(const std::string& caminho) {
    std::ofstream arquivo(caminho, std::ios::trunc);
    arquivo << "jogadores=" << (doisJogadores ? (modoRivais ? "inimigos" : "amigos") : "1") << '\n'
            << "p1=" << nomeDispositivo(dispositivoP1) << '\n'
            << "p2=" << nomeDispositivo(dispositivoP2) << '\n';
    return bool(arquivo);
}

void carregarConfiguracao(const std::string& caminho) {
    doisJogadores = modoRivais = false;
    dispositivoP1 = dispositivoP2 = TECLADO;
    std::ifstream arquivo(caminho);
    std::string linha;
    while (std::getline(arquivo, linha)) {
        size_t igual = linha.find('=');
        if (igual == std::string::npos) continue;
        std::string chave = linha.substr(0, igual), valor = linha.substr(igual + 1);
        if (!valor.empty() && valor.back() == '\r') valor.pop_back();
        if (chave == "jogadores") { doisJogadores = valor == "amigos" || valor == "inimigos"; modoRivais = valor == "inimigos"; }
        else if (chave == "p1") dispositivoP1 = valor == "joystick" ? JOYSTICK : TECLADO;
        else if (chave == "p2") dispositivoP2 = valor == "joystick" ? JOYSTICK : TECLADO;
    }
    validarAtribuicao();
}

void mostrar(const char* titulo) {
    std::printf("%-34s jogadores=%d  P1=%-8s", titulo, doisJogadores ? 2 : 1, nomeDispositivo(dispositivoP1));
    if (doisJogadores) std::printf("  P2=%s", nomeDispositivo(dispositivoP2));
    std::printf("\n");
}

int main() {
    const std::string caminho = "controles_aula33.cfg";
    joystickVirtual = true;
    atualizarConexao(false);
    lerTeclado('2', 0);       // AMIGOS
    lerTeclado('2', 0);       // P1 joystick + P2 teclado
    mostrar("escolhido na tela:");
    salvarConfiguracao(caminho);

    doisJogadores = false; dispositivoP1 = dispositivoP2 = TECLADO;
    carregarConfiguracao(caminho);
    mostrar("lido do arquivo, joystick ligado:");

    joystickVirtual = false;
    atualizarConexao(false);
    carregarConfiguracao(caminho);
    mostrar("lido do arquivo, sem joystick:");

    { std::ofstream estragado(caminho, std::ios::trunc); estragado << "jogadores=amigos\np1=joystick\np2=joystick\n"; }
    joystickVirtual = true;
    atualizarConexao(false);
    carregarConfiguracao(caminho);
    mostrar("arquivo com os dois no joystick:");

    DeleteFileA(caminho.c_str());
    return 0;
}
