// Marcos jogáveis: um arquivo, três jogos.
//
//   g++ -std=c++17 -DNIVEL=1 marcos/jogo_incremental.cpp -o Marco_1.exe -lgdiplus -mwindows
//
// NIVEL 1 (depois das Aulas 06, 07 e 18): arena 13 x 15, passo fixo de 8 ms,
//         casa lógica + posição desenhada, curva só no centro, animação pela distância.
// NIVEL 2 (depois das Aulas 08, 09 e 19): bomba com fusível de 2200 ms, chama
//         recursiva que atravessa caixas, 3 vidas.
// NIVEL 3 (depois das Aulas 10, 12, 20 e 27): três inimigos que perseguem com a
//         BFS do jogo, portal depois do último inimigo, duas fases.
//
// Os nomes e as regras são os de projeto_final/bomberman.cpp; o que ficou de
// fora está listado em marcos/README.md. Os blocos #if mostram o que cada nível acrescenta.
#include <windows.h>
#include <gdiplus.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>
#ifndef NIVEL
#define NIVEL 1
#endif

const int LINHAS = 13, COLUNAS = 15, TAMANHO_CELULA = 44, ALTURA_CABECALHO = 40;
const int INTERVALO_MOVIMENTO_MS = 280;
const float VELOCIDADE_VISUAL_CELS_SEG = 3.6f;
const int TEMPO_BOMBA_MS = 2200, TEMPO_EXPLOSAO_MS = 650, ALCANCE_EXPLOSAO = 2;
const int TOTAL_FASES_MARCO = 2, VIDAS_BASE = 3;

enum TipoCelula { VAZIO, PAREDE_SOLIDA, PAREDE_FRAGIL, PORTAL };
enum EstadoJogo { JOGANDO, VITORIA };
struct Posicao { int linha, coluna; };

TipoCelula mapa[LINHAS][COLUNAS];
std::mt19937 gerador(1);
EstadoJogo estado = JOGANDO;
int faseAtual = 1, vidas = VIDAS_BASE;

Posicao jogador{1, 1}, direcaoJogador{1, 0};
float jogadorVisualL = 1, jogadorVisualC = 1, distanciaAnimacao = 0;
int tempoMovimentoMs = 0, quadroAnimJogador = 1;
bool teclaPressionada[256] = {};
unsigned long long ordemTecla[256] = {}, proximaOrdemTecla = 0;

bool iguais(Posicao a, Posicao b) { return a.linha == b.linha && a.coluna == b.coluna; }
bool dentro(int l, int c) { return l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS; }
bool livre(int l, int c) { return dentro(l, c) && (mapa[l][c] == VAZIO || mapa[l][c] == PORTAL); }

// ---------- Aula 02: a arena (com limparArea corrigida) ----------
void criarMapa() {
    gerador.seed(100 + faseAtual);                 // semente fixa: a mesma arena a cada tentativa
    std::uniform_int_distribution<int> chance(0, 99);
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++) {
            bool borda = l == 0 || l == LINHAS - 1 || c == 0 || c == COLUNAS - 1;
            bool pilar = l % 2 == 0 && c % 2 == 0;
            if (borda || pilar) mapa[l][c] = PAREDE_SOLIDA;
            else if (chance(gerador) < 36) mapa[l][c] = PAREDE_FRAGIL;
            else mapa[l][c] = VAZIO;
        }
}
void limparArea(int linha, int coluna) {
    mapa[linha][coluna] = VAZIO;
    const int dl[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
    for (int d = 0; d < 4; d++)
        if (mapa[linha + dl[d]][coluna + dc[d]] == PAREDE_FRAGIL) mapa[linha + dl[d]][coluna + dc[d]] = VAZIO;
}

// ---------- Aula 08: bombas ----------
#if NIVEL >= 2
struct Bomba { Posicao posicao; bool explodindo = false; int tempoMs = TEMPO_BOMBA_MS; std::vector<Posicao> celulasExplosao; };
std::vector<Bomba> bombas;
bool bombaEm(Posicao p) { for (const Bomba& b : bombas) if (iguais(b.posicao, p)) return true; return false; }
#else
bool bombaEm(Posicao) { return false; }
#endif

// ---------- Aula 07: movimento ----------
bool podeAndarPara(Posicao destino) { return !bombaEm(destino) && livre(destino.linha, destino.coluna); }
bool visualNoDestino(float vl, float vc, Posicao alvo) {
    return std::fabs(vl - alvo.linha) < 0.0001f && std::fabs(vc - alvo.coluna) < 0.0001f;
}
float avancarVisual(float& vl, float& vc, Posicao alvo, int tempoMs) {
    float dl = alvo.linha - vl, dc = alvo.coluna - vc;
    float distancia = std::sqrt(dl * dl + dc * dc);
    if (distancia < 0.0001f) { vl = float(alvo.linha); vc = float(alvo.coluna); return 0; }
    float passo = std::min(distancia, VELOCIDADE_VISUAL_CELS_SEG * tempoMs / 1000.0f);
    vl += dl / distancia * passo;
    vc += dc / distancia * passo;
    return passo;
}
int quadroCaminhada(float distancia, bool andando) {
    const int ciclo[] = {0, 1, 0, 2};
    return andando ? ciclo[static_cast<int>(distancia * 2.5f) % 4] : 1;
}
void moverJogador(int dl, int dc) {
    if (estado != JOGANDO || tempoMovimentoMs > 0 || !visualNoDestino(jogadorVisualL, jogadorVisualC, jogador)) return;
    direcaoJogador = {dl, dc};
    Posicao origem = jogador;
    Posicao destino{jogador.linha + dl, jogador.coluna + dc};
    if (podeAndarPara(destino)) jogador = destino;
    tempoMovimentoMs = iguais(origem, jogador) ? 0 : INTERVALO_MOVIMENTO_MS;
}
// Aula 06: a tecla de direção apertada por último vence.
void processarMovimentoContinuo() {
    struct Entrada { int tecla, dl, dc; };
    const Entrada teclas[] = {{'W', -1, 0}, {'S', 1, 0}, {'A', 0, -1}, {'D', 0, 1},
                              {VK_UP, -1, 0}, {VK_DOWN, 1, 0}, {VK_LEFT, 0, -1}, {VK_RIGHT, 0, 1}};
    const Entrada* escolhida = nullptr;
    for (const Entrada& e : teclas)
        if (teclaPressionada[e.tecla] && (!escolhida || ordemTecla[e.tecla] > ordemTecla[escolhida->tecla])) escolhida = &e;
    if (escolhida) moverJogador(escolhida->dl, escolhida->dc);
}

// ---------- Aula 08: bombas (continuação) ----------
#if NIVEL >= 2
void propagarExplosao(std::vector<Posicao>& celulas, int dl, int dc, Posicao posAtual, int alcanceRestante) {
    if (alcanceRestante <= 0) return;
    Posicao proxima{posAtual.linha + dl, posAtual.coluna + dc};
    if (!dentro(proxima.linha, proxima.coluna) || mapa[proxima.linha][proxima.coluna] == PAREDE_SOLIDA) return;
    celulas.push_back(proxima);
    if (mapa[proxima.linha][proxima.coluna] == PAREDE_FRAGIL) mapa[proxima.linha][proxima.coluna] = VAZIO;
    propagarExplosao(celulas, dl, dc, proxima, alcanceRestante - 1);
}
void iniciarExplosaoBomba(Bomba& b) {
    b.explodindo = true;
    b.tempoMs = TEMPO_EXPLOSAO_MS;
    b.celulasExplosao = {b.posicao};
    propagarExplosao(b.celulasExplosao, -1, 0, b.posicao, ALCANCE_EXPLOSAO);
    propagarExplosao(b.celulasExplosao, 1, 0, b.posicao, ALCANCE_EXPLOSAO);
    propagarExplosao(b.celulasExplosao, 0, -1, b.posicao, ALCANCE_EXPLOSAO);
    propagarExplosao(b.celulasExplosao, 0, 1, b.posicao, ALCANCE_EXPLOSAO);
}
void colocarBomba() {
    if (estado != JOGANDO || !bombas.empty()) return;   // limite de 1 bomba (o jogo usa 1 + bombasExtras)
    Bomba nova;
    nova.posicao = jogador;
    bombas.push_back(nova);
}
void atualizarBombas(int tempoMs) {
    for (Bomba& b : bombas)
        if (!b.explodindo && (b.tempoMs -= tempoMs) <= 0) iniciarExplosaoBomba(b);
    for (size_t i = 0; i < bombas.size();) {
        if (bombas[i].explodindo && (bombas[i].tempoMs -= tempoMs) <= 0) { bombas.erase(bombas.begin() + i); continue; }
        i++;
    }
}
// Toque com a chama usa a posição desenhada, com caixa de meia célula (Aula 07).
bool naChama(float l, float c) {
    for (const Bomba& b : bombas)
        if (b.explodindo)
            for (const Posicao& p : b.celulasExplosao)
                if (std::fabs(l - p.linha) < .75f && std::fabs(c - p.coluna) < .75f) return true;
    return false;
}
#endif

// ---------- Aulas 10 e 12: inimigos, portal e fases ----------
#if NIVEL >= 3
struct Inimigo { Posicao posicao; float visualL, visualC; bool vivo = true; int tempoProximoMovimentoMs = 600;
                 Posicao direcao{1, 0}; float distanciaAnimacao = 0; int quadroAnim = 1; };
std::vector<Inimigo> inimigos;
bool portalAtivo = false;

template <typename EhAlvo, typename PodeAndarPor>
Posicao bfsProximoPasso(Posicao origem, EhAlvo ehAlvo, PodeAndarPor podeAndarPor) {
    const int direcoes[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool visitado[LINHAS][COLUNAS]{};
    Posicao anterior[LINHAS][COLUNAS];
    std::queue<Posicao> fila;
    fila.push(origem);
    visitado[origem.linha][origem.coluna] = true;
    Posicao objetivo = origem;
    bool encontrou = false;
    while (!fila.empty() && !encontrou) {
        Posicao atual = fila.front(); fila.pop();
        if (ehAlvo(atual)) { objetivo = atual; encontrou = true; break; }
        for (const auto& d : direcoes) {
            Posicao proxima{atual.linha + d[0], atual.coluna + d[1]};
            if (!podeAndarPor(proxima) || visitado[proxima.linha][proxima.coluna]) continue;
            visitado[proxima.linha][proxima.coluna] = true;
            anterior[proxima.linha][proxima.coluna] = atual;
            fila.push(proxima);
        }
    }
    if (!encontrou || iguais(objetivo, origem)) return origem;
    while (!iguais(anterior[objetivo.linha][objetivo.coluna], origem)) objetivo = anterior[objetivo.linha][objetivo.coluna];
    return objetivo;
}
bool temInimigo(Posicao p, int ignorar) {
    for (int i = 0; i < int(inimigos.size()); i++) if (i != ignorar && inimigos[i].vivo && iguais(inimigos[i].posicao, p)) return true;
    return false;
}
void moverInimigos(int tempoMs) {
    std::uniform_int_distribution<int> intervalo(420, 780);   // INTERVALO_INIMIGO_MIN/MAX_MS do jogo
    for (int i = 0; i < int(inimigos.size()); i++) {
        Inimigo& in = inimigos[i];
        if (!in.vivo || (in.tempoProximoMovimentoMs -= tempoMs) > 0) continue;
        in.tempoProximoMovimentoMs = intervalo(gerador);
        Posicao alvo = bfsProximoPasso(in.posicao, [&](Posicao p) { return iguais(p, jogador); },
                                       [](Posicao p) { return podeAndarPara(p); });
        if (iguais(alvo, in.posicao) || temInimigo(alvo, i)) continue;
        in.direcao = {alvo.linha - in.posicao.linha, alvo.coluna - in.posicao.coluna};
        in.posicao = alvo;
    }
}
void colocarPortal() {
    std::vector<Posicao> livres;
    for (int l = 1; l < LINHAS - 1; l++)
        for (int c = 1; c < COLUNAS - 1; c++)
            if (mapa[l][c] == VAZIO && !iguais({l, c}, jogador)) livres.push_back({l, c});
    if (livres.empty()) return;
    Posicao p = livres[std::uniform_int_distribution<int>(0, int(livres.size()) - 1)(gerador)];
    mapa[p.linha][p.coluna] = PORTAL;
    portalAtivo = true;
}
#endif

void montarFase() {
    criarMapa();
    const Posicao inicios[] = {{1, 1}, {LINHAS - 2, COLUNAS - 2}, {1, COLUNAS - 2}, {LINHAS - 2, 1}};
    for (const Posicao& p : inicios) limparArea(p.linha, p.coluna);
    jogador = inicios[0];
    jogadorVisualL = float(jogador.linha); jogadorVisualC = float(jogador.coluna);
    direcaoJogador = {1, 0};
    tempoMovimentoMs = 0;
    estado = JOGANDO;
#if NIVEL >= 2
    bombas.clear();
#endif
#if NIVEL >= 3
    inimigos.clear();
    for (int i = 1; i <= 3; i++) inimigos.push_back({inicios[i], float(inicios[i].linha), float(inicios[i].coluna)});
    portalAtivo = false;
#endif
}
void reiniciarFase() { montarFase(); }
void novaPartida() { faseAtual = 1; vidas = VIDAS_BASE; montarFase(); }

void atualizar(int tempoMs) {
    if (estado != JOGANDO) return;
    tempoMovimentoMs = std::max(0, tempoMovimentoMs - tempoMs);
    processarMovimentoContinuo();
#if NIVEL >= 2
    atualizarBombas(tempoMs);
#endif
#if NIVEL >= 3
    moverInimigos(tempoMs);
    for (Inimigo& in : inimigos) {
        if (!in.vivo) continue;
        float passo = avancarVisual(in.visualL, in.visualC, in.posicao, tempoMs);
        in.distanciaAnimacao += passo;
        in.quadroAnim = quadroCaminhada(in.distanciaAnimacao, passo > 0);
    }
#endif
    float passo = avancarVisual(jogadorVisualL, jogadorVisualC, jogador, tempoMs);
    distanciaAnimacao += passo;
    quadroAnimJogador = quadroCaminhada(distanciaAnimacao, passo > 0);

    bool atingido = false;
#if NIVEL >= 2
    atingido = naChama(jogadorVisualL, jogadorVisualC);
#endif
#if NIVEL >= 3
    for (Inimigo& in : inimigos) {
        if (!in.vivo) continue;
        if (naChama(in.visualL, in.visualC)) { in.vivo = false; continue; }
        if (std::fabs(in.visualL - jogadorVisualL) < .5f && std::fabs(in.visualC - jogadorVisualC) < .5f) atingido = true;
    }
    bool todosMortos = std::none_of(inimigos.begin(), inimigos.end(), [](const Inimigo& in) { return in.vivo; });
    if (todosMortos && !portalAtivo) colocarPortal();
    if (visualNoDestino(jogadorVisualL, jogadorVisualC, jogador) && mapa[jogador.linha][jogador.coluna] == PORTAL) {
        if (faseAtual < TOTAL_FASES_MARCO) { faseAtual++; vidas = VIDAS_BASE; montarFase(); }
        else estado = VITORIA;
        return;
    }
#endif
    if (atingido) {
        if (--vidas <= 0) novaPartida();
        else reiniciarFase();
    }
}

// ---------- desenho: tiles e sprites reais, num backbuffer (Aula 24) ----------
ULONG_PTR tokenGdiPlus = 0;
std::unique_ptr<Gdiplus::Bitmap> atlasArena, folhaJogador, folhaInimigo, folhaBomba, folhaChama;
std::wstring pastaAssets() {
    for (const wchar_t* pasta : {L"projeto_final\\assets\\", L"..\\projeto_final\\assets\\", L"..\\..\\projeto_final\\assets\\"})
        if (GetFileAttributesW(pasta) != INVALID_FILE_ATTRIBUTES) return pasta;
    return L"projeto_final\\assets\\";
}
std::unique_ptr<Gdiplus::Bitmap> carregarSprite(const wchar_t* nome) {
    auto imagem = std::make_unique<Gdiplus::Bitmap>((pastaAssets() + nome).c_str());
    return imagem->GetLastStatus() == Gdiplus::Ok ? std::move(imagem) : nullptr;
}
void carregarAssets() {
    atlasArena = carregarSprite(L"SNES - Super Bomberman - Story Mode Stages - Stage 2-5.png");   // temaDaFase(1)
    folhaJogador = carregarSprite(L"personagem_branco.png");
    folhaInimigo = carregarSprite(L"inimigo_direcional.png");
    folhaBomba = carregarSprite(L"bomba_animada.png");
    folhaChama = carregarSprite(L"explosao_animada.png");
}
void liberarAssets() { atlasArena.reset(); folhaJogador.reset(); folhaInimigo.reset(); folhaBomba.reset(); folhaChama.reset(); }

int linhaDaFolhaParaDirecao(Posicao d) { return d.linha == 1 ? 0 : d.coluna == 1 ? 1 : d.linha == -1 ? 2 : 3; }
void desenharPersonagem(Gdiplus::Graphics& g, Gdiplus::Bitmap* folha, float l, float c, Posicao direcao, int quadro) {
    int x = int(c * TAMANHO_CELULA), y = ALTURA_CABECALHO + int(l * TAMANHO_CELULA);
    Gdiplus::SolidBrush sombra(Gdiplus::Color(105, 0, 0, 0));
    g.FillEllipse(&sombra, x + 8, y + TAMANHO_CELULA - 12, TAMANHO_CELULA - 16, 9);
    if (!folha) return;
    int largura = 38, altura = 57;   // 16 x 24 ampliado; os pés no chão da célula
    g.DrawImage(folha, Gdiplus::Rect(x + (TAMANHO_CELULA - largura) / 2, y + TAMANHO_CELULA - 3 - altura, largura, altura),
                quadro * 16, linhaDaFolhaParaDirecao(direcao) * 24, 16, 24, Gdiplus::UnitPixel);
}
void desenharCena(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 11, 16, 28));
    g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    for (int l = 0; l < LINHAS; l++)
        for (int c = 0; c < COLUNAS; c++) {
            Gdiplus::Rect destino(c * TAMANHO_CELULA, ALTURA_CABECALHO + l * TAMANHO_CELULA, TAMANHO_CELULA, TAMANHO_CELULA);
            Gdiplus::Rect origem = mapa[l][c] == PAREDE_SOLIDA ? Gdiplus::Rect(16, 16, 16, 16)
                                 : mapa[l][c] == PAREDE_FRAGIL ? Gdiplus::Rect(48, 32, 16, 16) : Gdiplus::Rect(96, 48, 16, 16);
            if (atlasArena) g.DrawImage(atlasArena.get(), destino, origem.X, origem.Y, origem.Width, origem.Height, Gdiplus::UnitPixel);
            if (mapa[l][c] == PORTAL) {
                Gdiplus::SolidBrush roxo(Gdiplus::Color(170, 150, 40, 200));
                g.FillEllipse(&roxo, destino.X + 6, destino.Y + 6, destino.Width - 12, destino.Height - 12);
            }
        }
#if NIVEL >= 2
    for (const Bomba& b : bombas) {
        if (b.explodindo) {
            for (const Posicao& p : b.celulasExplosao)
                if (folhaChama) g.DrawImage(folhaChama.get(), Gdiplus::Rect(p.coluna * TAMANHO_CELULA, ALTURA_CABECALHO + p.linha * TAMANHO_CELULA,
                                                                             TAMANHO_CELULA, TAMANHO_CELULA), 0, 0, 16, 16, Gdiplus::UnitPixel);
        } else if (folhaBomba) {
            int quadro = ((TEMPO_BOMBA_MS - b.tempoMs) / 125) % 4;   // a bomba pulsa
            g.DrawImage(folhaBomba.get(), Gdiplus::Rect(b.posicao.coluna * TAMANHO_CELULA + 4, ALTURA_CABECALHO + b.posicao.linha * TAMANHO_CELULA + 4,
                                                        TAMANHO_CELULA - 8, TAMANHO_CELULA - 8), quadro * 16, 0, 16, 16, Gdiplus::UnitPixel);
        }
    }
#endif
#if NIVEL >= 3
    for (const Inimigo& in : inimigos)
        if (in.vivo) desenharPersonagem(g, folhaInimigo.get(), in.visualL, in.visualC, in.direcao, in.quadroAnim);
#endif
    desenharPersonagem(g, folhaJogador.get(), jogadorVisualL, jogadorVisualC, direcaoJogador, quadroAnimJogador);

    Gdiplus::Font fonte(L"Segoe UI", 16, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush branco(Gdiplus::Color(255, 220, 230, 240)), verde(Gdiplus::Color(255, 98, 235, 199));
    std::wstring hud = L"MARCO " + std::to_wstring(NIVEL) + L"   fase " + std::to_wstring(faseAtual);
#if NIVEL >= 2
    hud += L"   vidas " + std::to_wstring(vidas) + L"   ESPACO bomba";
#endif
    hud += L"   SETAS/WASD andam   R reinicia";
    g.DrawString(hud.c_str(), -1, &fonte, Gdiplus::PointF(10, 10), &branco);
    if (estado == VITORIA) {
        Gdiplus::Font grande(L"Segoe UI", 40, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        g.DrawString(L"VITORIA!  R joga de novo", -1, &grande, Gdiplus::PointF(90, 290), &verde);
    }
}

HDC bufferDC = nullptr; HBITMAP bufferBitmap = nullptr;
void desenhar(HWND janela) {
    PAINTSTRUCT ps;
    HDC destino = BeginPaint(janela, &ps);
    RECT r; GetClientRect(janela, &r);
    if (!bufferDC) {
        bufferDC = CreateCompatibleDC(destino);
        bufferBitmap = CreateCompatibleBitmap(destino, r.right, r.bottom);
        SelectObject(bufferDC, bufferBitmap);
    }
    { Gdiplus::Graphics g(bufferDC); desenharCena(g); }
    BitBlt(destino, 0, 0, r.right, r.bottom, bufferDC, 0, 0, SRCCOPY);
    EndPaint(janela, &ps);
}

LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    static auto ultimoInstante = std::chrono::steady_clock::now();
    static int acumuladoMs = 0;
    switch (mensagem) {
    case WM_CREATE:
        carregarAssets();
        novaPartida();
        SetTimer(janela, 1, 16, nullptr);
        return 0;
    case WM_TIMER: {   // Aula 06: tempo real medido, simulado em passos de 8 ms
        auto agora = std::chrono::steady_clock::now();
        int decorrido = int(std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoInstante).count());
        ultimoInstante = agora;
        acumuladoMs += std::min(decorrido, 100);
        while (acumuladoMs >= 8) { atualizar(8); acumuladoMs -= 8; }
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam < 256) {
            if (!teclaPressionada[wParam]) ordemTecla[wParam] = ++proximaOrdemTecla;
            teclaPressionada[wParam] = true;
        }
        if (lParam & (1 << 30)) return 0;   // repetição automática: só movimento usa tecla segurada
        if (wParam == VK_ESCAPE) DestroyWindow(janela);
        else if (wParam == 'R') novaPartida();
#if NIVEL >= 2
        else if (wParam == VK_SPACE) colocarBomba();
#endif
        return 0;
    case WM_KEYUP:
        if (wParam < 256) teclaPressionada[wParam] = false;
        return 0;
    case WM_KILLFOCUS:
        std::fill(std::begin(teclaPressionada), std::end(teclaPressionada), false);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        desenhar(janela);
        return 0;
    case WM_DESTROY:
        KillTimer(janela, 1);
        if (bufferDC) { DeleteDC(bufferDC); DeleteObject(bufferBitmap); }
        liberarAssets();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;
    classe.hInstance = instancia;
    classe.lpszClassName = L"MarcoBomberman";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 2; }
    RECT area{0, 0, COLUNAS * TAMANHO_CELULA, ALTURA_CABECALHO + LINHAS * TAMANHO_CELULA};
    DWORD estilo = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;   // tamanho fixo: o buffer não muda
    AdjustWindowRect(&area, estilo, FALSE);
    std::wstring titulo = L"Bomberman - Marco " + std::to_wstring(NIVEL);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, titulo.c_str(), estilo, CW_USEDEFAULT, CW_USEDEFAULT,
                                  area.right - area.left, area.bottom - area.top, nullptr, nullptr, instancia, nullptr);
    if (!janela) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 3; }
    ShowWindow(janela, exibir);
    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) { TranslateMessage(&evento); DispatchMessageW(&evento); }
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    return 0;
}
