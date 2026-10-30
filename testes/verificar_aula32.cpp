// Verifica a tela de ranking da Aula 32 sem abrir janela e exporta as telas para o roteiro.
// As "teclas" são chamadas diretas às mesmas funções que WM_KEYDOWN, WM_CHAR e
// WM_LBUTTONUP chamam; nada é enviado ao sistema. O CSV é temporário.
// Rodar na raiz do curso (validar_aulas.ps1 faz isso).
#define WinMain WinMain_da_aula
#include "../aulas/Aula_32_tela_de_ranking_crud/Aula_32_tela_de_ranking_crud.cpp"
#undef WinMain
#include <cstdio>
#include <sstream>

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

void digitar(const wchar_t* texto) { for (const wchar_t* c = texto; *c; ++c) digitarNomeJogador(*c); }
void apagarTudo() { lerTeclado(VK_END, 0); while (!nomeEmEdicao.empty()) digitarNomeJogador(L'\b'); }
int posicaoDe(const char* nome) {
    auto r = lerRanking();
    for (int i = 0; i < int(r.size()); ++i) if (mesmoNome(r[i].jogador, nome)) return i;
    return -1;
}
int pontosDe(const char* nome) { int i = posicaoDe(nome); return i < 0 ? -1 : lerRanking()[i].pontuacao; }
std::string conteudo(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    std::stringstream texto;
    texto << arquivo.rdbuf();
    return texto.str();
}
bool existe(const std::string& caminho) { return GetFileAttributesA(caminho.c_str()) != INVALID_FILE_ATTRIBUTES; }

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    ajustarAreaDesenho(LARGURA, ALTURA);
    CreateDirectoryA("build", nullptr);
    CreateDirectoryA("build\\internos", nullptr);
    CreateDirectoryA("build\\internos\\testes", nullptr);
    caminhoRanking = "build\\internos\\testes\\ranking_verificar_aula32.csv";
    DeleteFileA(caminhoRanking.c_str());
    DeleteFileA((caminhoRanking + ".bak").c_str());
    const wchar_t* pasta = L"aulas\\Aula_32_tela_de_ranking_crud\\imagens\\";
    CreateDirectoryW(pasta, nullptr);
    auto caminho = [&](const wchar_t* nome) { return std::wstring(pasta) + nome; };

    // --- Read ---
    criarRankingDeExemplo();
    auto r = lerRanking();
    conferir(r.size() == 12, "o exemplo tem 12 registros (duas paginas)");
    conferir(r[0].jogador == "Davi" && r[1].jogador == "Bia" && r[2].jogador == "Ana", "empate em 900: menos bombas, depois menos movimentos");
    conferir(posicaoDe("eva; a \"rápida\"") == 6, "nome com ; e aspas volta igual do CSV, achado sem diferenciar maiusculas");
    conferir(!existe(caminhoRanking + ".bak"), "o exemplo nao deixa .bak");
    fotografar(caminho(L"ranking_1_lista.png").c_str());

    // --- navegação e página ---
    lerTeclado(VK_NEXT, 0);
    conferir(rankingSelecionado == 10, "PgDn pula 10 registros e abre a pagina 2");
    lerTeclado(VK_NEXT, 0);
    conferir(rankingSelecionado == 11, "PgDn na ultima pagina para no ultimo registro");
    fotografar(caminho(L"ranking_4_pagina2.png").c_str());
    lerTeclado(VK_HOME, 0);
    clicar(100, TOPO_LINHAS + 3 * ALTURA_LINHA + 5);
    conferir(rankingSelecionado == 3, "clique na 4a linha seleciona Caio");

    // --- Update ---
    lerTeclado('E', 0);
    conferir(editandoNome && nomeEmEdicao == L"Caio", "E abre o campo com o nome atual");
    apagarTudo();
    digitar(L"  BIA ");
    lerTeclado(VK_RETURN, 0);
    conferir(editandoNome && pontosDe("Caio") == 700, "renomear para ' BIA ' e recusado: Bia ja existe (mesma chave)");
    apagarTudo();
    digitar(L"Caio  Lima");
    lerTeclado(VK_LEFT, 0);
    lerTeclado(VK_LEFT, 0);
    fotografar(caminho(L"ranking_2_renomear.png").c_str());
    lerTeclado(VK_RETURN, 0);
    conferir(!editandoNome && pontosDe("Caio Lima") == 700 && posicaoDe("Caio") < 0, "renomear preserva os 700 pontos e limpa o espaco duplo");
    conferir(existe(caminhoRanking + ".bak"), "a primeira gravacao cria o .bak");
    conferir(rankingSelecionado == posicaoDe("Caio Lima"), "a selecao acompanha o nome renomeado");
    lerTeclado('E', 0);
    lerTeclado(VK_ESCAPE, 0);
    conferir(!editandoNome && pontosDe("Caio Lima") == 700, "ESC cancela sem gravar");
    lerTeclado('E', 0);
    lerTeclado('Q', 0);   // letras não passam por lerTeclado durante a edição
    conferir(editandoNome && nomeEmEdicao == L"Caio Lima", "atalho de letra nao dispara durante a digitacao");
    lerTeclado(VK_ESCAPE, 0);

    // --- Delete ---
    lerTeclado(VK_HOME, 0);
    lerTeclado(VK_DOWN, 0);
    lerTeclado(VK_DELETE, 0);
    conferir(confirmandoExclusao, "DEL pergunta antes de excluir");
    fotografar(caminho(L"ranking_3_excluir.png").c_str());
    lerTeclado(VK_DOWN, 0);
    conferir(rankingSelecionado == 1, "com a pergunta aberta, as setas nao mudam a selecao");
    lerTeclado('N', 0);
    conferir(!confirmandoExclusao && posicaoDe("Bia") == 1, "N cancela: Bia continua");
    lerTeclado(VK_DELETE, 0);
    clicar(250, 320);
    conferir(posicaoDe("Bia") < 0 && lerRanking().size() == 11, "clique em EXCLUIR apaga Bia");
    std::string arquivo = conteudo(caminhoRanking);
    conferir(arquivo.find(";Bia;") == std::string::npos && arquivo.find("\"Eva; a \"\"R") != std::string::npos,
             "no arquivo: Bia sumiu e o nome com ; segue entre aspas");

    // --- Create: um registro por nome ---
    RegistroPontuacao pior{"29/10/2026", " ana", 100, 0, 10}, melhor{"29/10/2026", "ANA", 950, 1, 10};
    salvarRegistroPartida(pior);
    conferir(pontosDe("Ana") == 900 && lerRanking().size() == 11, "partida pior de ' ana' nao cria nem substitui");
    salvarRegistroPartida(melhor);
    conferir(posicaoDe("ana") == 0 && pontosDe("Ana") == 950 && lerRanking().size() == 11, "partida melhor de 'ANA' substitui e sobe ao topo");
    RegistroPontuacao bia{"29/10/2026", "Bia", 200, 0, 5};
    salvarRegistroPartida(bia);
    conferir(pontosDe("Bia") == 200, "excluida, Bia recomeca do zero");

    // --- Read robusto: arquivo editado à mão ---
    {
        std::ofstream mao(caminhoRanking, std::ios::app);
        mao << "01/10/2026;Zed;muito;1;1\n" << "01/10/2026;davi;10;0;0\n" << "01/10/2026;Yan;-5;1;1\n";
    }
    lerTeclado('R', 0);
    conferir(posicaoDe("Zed") < 0 && posicaoDe("Yan") < 0 && pontosDe("Davi") == 900, "R rele: linhas ruins caem e 'davi' duplicado nao vence Davi");
    conferir(!existe(caminhoRanking + ".tmp"), "nenhum .tmp fica para tras");

    DeleteFileA(caminhoRanking.c_str());
    DeleteFileA((caminhoRanking + ".bak").c_str());
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    std::printf(falhas ? "Aula 32: %d falha(s)\n" : "Aula 32: ranking verificado\n", falhas);
    return falhas ? 1 : 0;
}
