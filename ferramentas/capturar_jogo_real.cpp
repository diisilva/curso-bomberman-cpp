// Gera as capturas do jogo real usadas nos roteiros (pasta imagens_jogo_real/).
//
// O programa inclui projeto_final/bomberman.cpp, monta estados conhecidos
// (menus, partida, chefe, Mario, dois jogadores) e chama as MESMAS funções de
// desenho do jogo, só que num Bitmap em memória: nenhuma janela é aberta e
// nenhuma tecla é enviada ao sistema. Rode com ferramentas/capturar_jogo_real.ps1.
#include "../projeto_final/bomberman.cpp"
#include <cstdio>

namespace captura {

bool salvarPng(Gdiplus::Bitmap& imagem, const wstring& caminho) {
    UINT quantidade = 0, bytes = 0;
    Gdiplus::GetImageEncodersSize(&quantidade, &bytes);
    vector<unsigned char> memoria(bytes);
    auto* codecs = reinterpret_cast<Gdiplus::ImageCodecInfo*>(memoria.data());
    Gdiplus::GetImageEncoders(quantidade, bytes, codecs);
    for (UINT i = 0; i < quantidade; ++i)
        if (wcscmp(codecs[i].MimeType, L"image/png") == 0)
            return imagem.Save(caminho.c_str(), &codecs[i].Clsid, nullptr) == Gdiplus::Ok;
    return false;
}

// Mesmo corpo de desenhar(HWND), trocando o HDC da janela por um Bitmap.
void desenharQuadro(Gdiplus::Graphics& g) {
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    fundoInterface(g);
    if (modo == MENU) {
        switch (telaMenu) {
            case MENU_FASES: desenharSeletorFases(g); break;
            case MENU_PERSONAGENS: desenharPersonagens(g); break;
            case MENU_RANKING: desenharRanking(g); break;
            case MENU_DIFICULDADE: case MENU_DIFICULDADES_INFO: desenharEscolhaDificuldade(g); break;
            case MENU_COMO_JOGAR: desenharComoJogar(g); break;
            case MENU_PONTUACAO: desenharPontuacaoInfo(g); break;
            case MENU_QUANTIDADE_JOGADORES: desenharQuantidadeJogadores(g); break;
            case MENU_ATRIBUICAO_CONTROLES: desenharAtribuicaoControles(g); break;
            case MENU_NOMES: desenharNomes(g); break;
            default: desenharMenu(g); break;
        }
    } else {
        desenharHUD(g);
        desenharMapa(g);
        desenharPainelLateral(g);
        desenharRodape(g);
        desenharFimPartida(g);
    }
}

wstring pastaSaida;
int salvas = 0, falhas = 0;

void salvar(const wchar_t* nome) {
    Gdiplus::Bitmap imagem(LARGURA_JOGO, ALTURA_JOGO, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&imagem);
        desenharQuadro(g);
    }
    wstring caminho = pastaSaida + L"\\" + nome + L".png";
    if (salvarPng(imagem, caminho)) { ++salvas; wprintf(L"  %ls.png\n", nome); }
    else { ++falhas; wprintf(L"  FALHA %ls.png\n", nome); }
}

// Avança a simulação em passos de 8 ms (como WM_TIMER faz) até a condição valer.
template<class Condicao> bool simularAte(Condicao condicao, int limiteMs) {
    for (int t = 0; t < limiteMs; t += 8) {
        relogioAnimacaoMs += 8;
        atualizar(8);
        if (condicao()) return true;
    }
    return false;
}

void prepararNomes() {
    nomeJogador = L"Ana"; nomeConfirmadoRanking = chaveNomeRanking(wparaStr(nomeJogador));
    nomeJogador2 = L"Bruno"; nomeConfirmadoRanking2 = chaveNomeRanking(wparaStr(nomeJogador2));
}

void menus() {
    modo = MENU; doisJogadores = false; modoRivais = false;
    controleConectado = false; nomeControle.clear();
    telaMenu = MENU_PRINCIPAL; salvar(L"menu_principal");
    telaMenu = MENU_QUANTIDADE_JOGADORES; doisJogadores = true; salvar(L"menu_quantidade_jogadores");
    telaMenu = MENU_NOMES; editandoNome = true; campoNome = 2; cursorNome = nomeJogador2.size();
    salvar(L"menu_nomes");
    editandoNome = false;
    telaMenu = MENU_ATRIBUICAO_CONTROLES; salvar(L"menu_controles_sem_joystick");
    // Simula um controle conectado: só o estado muda, nenhum hardware é lido.
    controleConectado = true; origemControle = CONTROLE_XINPUT; nomeControle = L"Controle XInput";
    dispositivoP1 = TECLADO; dispositivoP2 = JOYSTICK;
    salvar(L"menu_controles_com_joystick");
    telaMenu = MENU_PRINCIPAL; salvar(L"menu_principal_com_joystick");
    controleConectado = false; nomeControle.clear(); dispositivoP2 = TECLADO;
    telaMenu = MENU_PERSONAGENS; jogadorEmSelecao = 2; personagemP1 = 0; personagemP2 = 2; salvar(L"menu_personagens");
    telaMenu = MENU_DIFICULDADE; dificuldade = MEDIO; salvar(L"menu_dificuldade");
    telaMenu = MENU_FASES; faseSelecionada = 6; salvar(L"menu_fases");
    telaMenu = MENU_RANKING; rankingSelecionado = 0; salvar(L"menu_ranking");
    telaMenu = MENU_COMO_JOGAR; salvar(L"menu_como_jogar");
    telaMenu = MENU_PONTUACAO; salvar(L"menu_pontuacao");
    doisJogadores = false; personagemP2 = 1;
}

void partidaComExplosao() {
    gerador.seed(2026);
    faseSelecionada = 1; doisJogadores = false;
    novaPartida(AUTOMATICO, MEDIO);
    salvar(L"partida_inicio_fase1");
    // Espera uma chama longa: ela mostra a propagação em cruz pelas caixas.
    simularAte([] {
        for (const Bomba& b : bombas) if (b.explodindo && b.celulasExplosao.size() >= 6) return true;
        return false;
    }, 60000);
    salvar(L"partida_explosao");
}

void partidaComItens() {
    gerador.seed(7);
    faseSelecionada = 3; doisJogadores = false;
    novaPartida(MANUAL, FACIL);
    for (Inimigo& i : inimigos) i.vivo = false;
    // Os oito itens no chão, ao redor do jogador, na ordem do enum TipoItem.
    Posicao livres[8]; int achados = 0;
    for (int l = 1; l < LINHAS - 1 && achados < 8; ++l)
        for (int c = 1; c < COLUNAS - 1 && achados < 8; ++c)
            if (mapa[l][c] == VAZIO && !(l == jogador.linha && c == jogador.coluna)) livres[achados++] = {l, c};
    for (int i = 0; i < achados; ++i) itensNoChao.push_back({livres[i], static_cast<TipoItem>(i)});
    possuiCartaChute = possuiCartaTeleporte = true; alcanceExplosao = 3; bombasExtras = 1;
    invalidarCacheMapa();
    salvar(L"partida_itens");
}

void partidaDoisJogadores() {
    gerador.seed(11);
    prepararNomes();
    faseSelecionada = 5; doisJogadores = true; modoRivais = false;
    novaPartida(MANUAL, MEDIO);
    salvar(L"partida_dois_jogadores");
    doisJogadores = false;
}

void partidaChefe(int fase, const wchar_t* nome) {
    gerador.seed(100 + fase);
    faseSelecionada = fase; doisJogadores = false;
    novaPartida(AUTOMATICO, MEDIO);
    for (Inimigo& i : inimigos) i.vivo = false;  // o chefe só entra depois dos inimigos comuns
    // Até o chefe surgir e atacar pela primeira vez.
    simularAte([] { return bossIniciado && boss.vivo && (!projeteisBoss.empty() || !perigoBoss.empty() || !fogosBoss.empty()); }, 40000);
    salvar(nome);
}

void partidaMario() {
    gerador.seed(42);
    faseSelecionada = 2; doisJogadores = false;
    novaPartida(MANUAL, FACIL);
    for (Inimigo& i : inimigos) i.vivo = false;
    if (marioAliado.escondido) {
        mapa[marioAliado.bloco.linha][marioAliado.bloco.coluna] = VAZIO;
        libertarMario(marioAliado.bloco, 0);
        invalidarCacheMapa();
    }
    simularAte([] { return marioAliado.ativo && marioAliado.andando; }, 3000);
    salvar(L"partida_mario");
}

void fimDePartida() {
    gerador.seed(5);
    faseSelecionada = TOTAL_FASES;
    novaPartida(MANUAL, FACIL);
    estado = VITORIA; tempoFimMs = 2000; pontos = 1840;
    salvar(L"partida_vitoria");
}

} // namespace captura

int main(int argc, char** argv) {
    if (argc < 2) { std::puts("uso: capturar_jogo_real <pasta de saida>"); return 2; }
    std::string saida = argv[1];
    captura::pastaSaida = wstring(saida.begin(), saida.end());
    CreateDirectoryW(captura::pastaSaida.c_str(), nullptr);

    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    carregarAssets();
    ajustarAreaDesenho(LARGURA_JOGO, ALTURA_JOGO);  // escala 1: uma unidade = um pixel
    captura::prepararNomes();

    captura::menus();
    captura::partidaComExplosao();
    captura::partidaComItens();
    captura::partidaDoisJogadores();
    captura::partidaChefe(2, L"partida_chefe_golem");
    captura::partidaChefe(4, L"partida_chefe_egg_hvc");
    captura::partidaChefe(6, L"partida_chefe_hornet");
    captura::partidaChefe(8, L"partida_chefe_guardiao");
    captura::partidaChefe(10, L"partida_chefe_bowser");
    captura::partidaMario();
    captura::fimDePartida();

    std::printf("%d capturas salvas, %d falhas\n", captura::salvas, captura::falhas);
    liberarAssets();
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    return captura::falhas ? 3 : 0;
}
