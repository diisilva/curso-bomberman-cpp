// Verifica os marcos sem abrir janela e exporta uma imagem de cada nível.
// Compile três vezes, com -DNIVEL=1, 2 e 3 (validar_marcos.ps1 faz isso), e rode na raiz do curso.
#define WinMain WinMain_do_marco
#include "../marcos/jogo_incremental.cpp"
#undef WinMain
#include <cstdio>
#include <cstring>

int falhas = 0;
void conferir(bool ok, const char* descricao) { std::printf("%s %s\n", ok ? "ok   " : "FALHA", descricao); if (!ok) ++falhas; }
void esperar(int ms) { for (int t = 0; t < ms; t += 8) atualizar(8); }

void fotografar(const wchar_t* caminho) {
    Gdiplus::Bitmap imagem(COLUNAS * TAMANHO_CELULA, ALTURA_CABECALHO + LINHAS * TAMANHO_CELULA, PixelFormat32bppARGB);
    { Gdiplus::Graphics g(&imagem); desenharCena(g); }
    UINT n = 0, bytes = 0;
    Gdiplus::GetImageEncodersSize(&n, &bytes);
    std::vector<unsigned char> memoria(bytes);
    auto* codecs = reinterpret_cast<Gdiplus::ImageCodecInfo*>(memoria.data());
    Gdiplus::GetImageEncoders(n, bytes, codecs);
    for (UINT i = 0; i < n; ++i) if (std::wcscmp(codecs[i].MimeType, L"image/png") == 0) imagem.Save(caminho, &codecs[i].Clsid, nullptr);
}

int main() {
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    carregarAssets();
    conferir(atlasArena && folhaJogador, "tiles e sprite do jogador carregados de projeto_final/assets");
    novaPartida();

    // Andar: segurar a seta para baixo por 300 ms anda uma casa e o desenho chega junto.
    teclaPressionada[VK_DOWN] = true; ordemTecla[VK_DOWN] = ++proximaOrdemTecla;
    esperar(8);
    conferir(jogador.linha == 2 && jogador.coluna == 1, "seta para baixo reserva (2,1) na hora");
    teclaPressionada[VK_DOWN] = false;
    esperar(300);
    conferir(visualNoDestino(jogadorVisualL, jogadorVisualC, jogador), "o desenho chega a (2,1) em menos de 300 ms");
#if NIVEL == 1
    fotografar(L"marcos/marco_1.png");
#endif
#if NIVEL >= 2
    colocarBomba();
    colocarBomba();
    conferir(bombas.size() == 1, "limite de uma bomba por vez");
    teclaPressionada[VK_UP] = true; ordemTecla[VK_UP] = ++proximaOrdemTecla;   // foge para (1,1)
    esperar(8);
    teclaPressionada[VK_UP] = false;
    teclaPressionada[VK_RIGHT] = true; ordemTecla[VK_RIGHT] = ++proximaOrdemTecla;
    esperar(600);                                                              // em (1,2): fora da linha e da coluna da bomba
    teclaPressionada[VK_RIGHT] = false;
    esperar(TEMPO_BOMBA_MS - 600);
    conferir(!bombas.empty() && bombas[0].explodindo, "a bomba explode depois de 2200 ms");
    conferir(vidas == VIDAS_BASE, "quem saiu da linha da bomba sobrevive");
    fotografar(NIVEL == 2 ? L"marcos/marco_2.png" : L"marcos/marco_3.png");
    esperar(TEMPO_EXPLOSAO_MS + 16);
    conferir(bombas.empty(), "a chama some depois de 650 ms");
    int vidasAntes = vidas;
    jogador = {5, 5}; jogadorVisualL = 5; jogadorVisualC = 5;
    mapa[5][5] = VAZIO;
    colocarBomba();
    esperar(TEMPO_BOMBA_MS + 16);
    conferir(vidas == vidasAntes - 1, "ficar na chama custa uma vida e reinicia a fase");
#endif
#if NIVEL >= 3
    for (Inimigo& in : inimigos) in.vivo = false;
    esperar(8);
    conferir(portalAtivo, "sem inimigos vivos, o portal aparece");
    for (int l = 0; l < LINHAS; l++) for (int c = 0; c < COLUNAS; c++)
        if (mapa[l][c] == PORTAL) { jogador = {l, c}; jogadorVisualL = float(l); jogadorVisualC = float(c); }
    esperar(8);
    conferir(faseAtual == 2 && vidas == VIDAS_BASE, "o portal leva a fase 2 e as vidas voltam a 3");
#endif
    liberarAssets();
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    std::printf("Marco %d: %s\n", NIVEL, falhas ? "HA FALHAS" : "verificado");
    return falhas ? 1 : 0;
}
