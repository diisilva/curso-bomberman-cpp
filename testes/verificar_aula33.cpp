// Verifica a configuração de controles da Aula 33 sem abrir janela e exporta as
// telas para o roteiro. O joystick é o virtual (tecla J); nenhum controle físico
// é lido, porque obterEstadoXInput fica nulo fora da janela.
// Rodar na raiz do curso (validar_aulas.ps1 faz isso).
#define WinMain WinMain_da_aula
#include "../aulas/Aula_33_configuracao_de_controles/Aula_33_configuracao_de_controles.cpp"
#undef WinMain
#include <cstdio>
#include <vector>

int falhas = 0;
void conferir(bool condicao, const char* descricao) {
    std::printf("%s %s\n", condicao ? "ok   " : "FALHA", descricao);
    if (!condicao) ++falhas;
}

void fotografar(const wchar_t* arquivo) {
    Gdiplus::Bitmap imagem(LARGURA, ALTURA, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&imagem);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        desenharCena(g);
    }
    UINT quantidade = 0, bytes = 0;
    Gdiplus::GetImageEncodersSize(&quantidade, &bytes);
    std::vector<unsigned char> memoria(bytes);
    auto* codecs = reinterpret_cast<Gdiplus::ImageCodecInfo*>(memoria.data());
    Gdiplus::GetImageEncoders(quantidade, bytes, codecs);
    for (UINT i = 0; i < quantidade; ++i)
        if (wcscmp(codecs[i].MimeType, L"image/png") == 0) imagem.Save(arquivo, &codecs[i].Clsid, nullptr);
}
void conectar(bool sim) { if (joystickVirtual != sim) lerTeclado('J', 0); }
void irParaControles(char quantidade) { tela = TELA_QUANTIDADE; lerTeclado(quantidade, 0); }

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    ajustarAreaDesenho(LARGURA, ALTURA);
    const wchar_t* pasta = L"aulas\\Aula_33_configuracao_de_controles\\imagens\\";
    CreateDirectoryW(pasta, nullptr);
    auto caminho = [&](const wchar_t* nome) { return std::wstring(pasta) + nome; };

    // --- quantidade ---
    lerTeclado(VK_RIGHT, 0);
    conferir(tela == TELA_QUANTIDADE && doisJogadores && !modoRivais, "seta para a direita marca AMIGOS sem sair da tela");
    fotografar(caminho(L"controles_1_quantidade.png").c_str());
    conectar(true);
    conferir(controleConectado && tela == TELA_QUANTIDADE && doisJogadores, "conectar o joystick nao cria nem tira jogador");
    conectar(false);

    // --- 1 jogador, sem joystick ---
    irParaControles('1');
    conferir(tela == TELA_CONTROLES && !doisJogadores, "1 abre os controles com um jogador");
    lerTeclado(VK_RIGHT, 0);
    conferir(dispositivoP1 == TECLADO && teclaDoJogador(false) == (TECLAS_WASD | TECLAS_SETAS),
             "sem joystick, nada se move: P1 fica com WASD e SETAS");
    fotografar(caminho(L"controles_2_sem_joystick.png").c_str());

    // --- 1 jogador, com joystick ---
    conectar(true);
    lerTeclado('L', 0);
    conferir(ladoDispositivo(JOYSTICK) == 0 && dispositivoP1 == TECLADO, "1P: direcional para a direita leva o joystick para SEM USO");
    lerTeclado('K', 0);
    conferir(dispositivoP1 == JOYSTICK && ladoDispositivo(TECLADO) == 0, "1P: direcional para a esquerda leva o joystick para P1 e o teclado sai");
    conferir(teclaDoJogador(false) == 0 && teclasDoJogador(1) == L"JOYSTICK (A/B bomba)", "1P no joystick: nenhuma tecla move P1");
    fotografar(caminho(L"controles_3_1p_joystick.png").c_str());
    conferir(donoDaTecla('W') == 0 && donoDaTecla(VK_SPACE) == 0, "1P no joystick: W e ESPACO nao movem ninguem");
    lerTeclado('1', 0);
    conferir(dispositivoP1 == TECLADO, "atalho 1 devolve P1 ao teclado");

    // --- 2 jogadores, com joystick ---
    irParaControles('2');
    conferir(doisJogadores && dispositivoP1 != dispositivoP2, "2P: ao abrir, P1 e P2 ficam com aparelhos diferentes");
    lerTeclado('2', 0);
    conferir(dispositivoP1 == JOYSTICK && dispositivoP2 == TECLADO, "2P: atalho 2 = P1 joystick + P2 teclado");
    conferir(teclaDoJogador(true) == (TECLAS_WASD | TECLAS_SETAS), "2P: P2 sozinho no teclado aceita WASD e SETAS");
    lerTeclado(VK_LEFT, 0);
    conferir(dispositivoP1 == TECLADO && dispositivoP2 == JOYSTICK, "2P: mover o teclado para P1 empurra o joystick para P2");
    lerTeclado('2', 0);
    fotografar(caminho(L"controles_4_2p_trocado.png").c_str());

    // --- todas as combinações respeitam a regra ---
    bool sempreDistintos = true;
    for (char q : {'2', '3'}) for (WPARAM t : {WPARAM(VK_LEFT), WPARAM(VK_RIGHT), WPARAM('K'), WPARAM('L'), WPARAM('1'), WPARAM('2')}) {
        irParaControles(q);
        lerTeclado(t, 0);
        sempreDistintos = sempreDistintos && dispositivoP1 != dispositivoP2;
    }
    conferir(sempreDistintos, "2P com joystick: nenhuma tecla deixa os dois no mesmo aparelho");

    // --- resumo e desconexão ---
    irParaControles('2');
    lerTeclado('2', 0);
    lerTeclado(VK_RETURN, 0);
    conferir(tela == TELA_RESUMO, "ENTER vai ao resumo");
    lerTeclado('W', 0);
    conferir(donoDaTecla('W') == 2 && donoDaTecla(VK_RETURN) == 2, "P1 no joystick: P2, sozinho no teclado, fica com W e com ENTER");
    fotografar(caminho(L"controles_5_resumo.png").c_str());
    conectar(false);
    conferir(dispositivoP1 == JOYSTICK && teclaDoJogador(false) == TECLAS_WASD && teclaDoJogador(true) == TECLAS_SETAS,
             "desconectou: a escolha fica guardada, mas o teclado se divide (P1 WASD, P2 SETAS)");
    conectar(true);
    conferir(controleNoJogador(1) && teclaDoJogador(false) == 0, "reconectou: P1 volta ao joystick sem escolher de novo");

    // --- 2P sem joystick: teclado dividido ---
    conectar(false);
    irParaControles('2');
    lerTeclado(VK_RIGHT, 0);
    conferir(dispositivoP1 == TECLADO && dispositivoP2 == TECLADO && teclaDoJogador(false) == TECLAS_WASD && teclaDoJogador(true) == TECLAS_SETAS,
             "2P sem joystick: P1 WASD + ESPACO, P2 SETAS + ENTER");

    // --- conectar com a tela aberta (diferença em relação ao jogo) ---
    conectar(true);
    conferir(ladoDispositivo(JOYSTICK) == 1, "conectar com a tela aberta em 2P ja coloca o joystick em P2");

    // --- clique ---
    clicar(30 + 2 * 240 + 50, 200);
    conferir(dispositivoP2 == TECLADO && dispositivoP1 == JOYSTICK, "clique na coluna P2 leva o teclado para P2");
    clicar(40, 345);
    conferir(dispositivoP1 == TECLADO, "clique no botao 1 escolhe P1 TECLADO + P2 JOYSTICK");

    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    std::printf(falhas ? "Aula 33: %d falha(s)\n" : "Aula 33: controles verificados\n", falhas);
    return falhas ? 1 : 0;
}
