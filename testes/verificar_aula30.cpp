// Verifica o menu da Aula 30 sem abrir janela e exporta as telas para o roteiro.
// As "teclas" são chamadas diretas às mesmas funções que WM_KEYDOWN, WM_CHAR e
// WM_LBUTTONUP chamam; nada é enviado ao sistema.
// Rodar na raiz do curso (validar_aulas.ps1 faz isso).
#define WinMain WinMain_da_aula
#include "../aulas/Aula_30_menu_completo/Aula_30_menu_completo.cpp"
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

void digitar(const wchar_t* texto) { for (const wchar_t* c = texto; *c; ++c) digitarNomeJogador(*c); }

int main() {
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    ajustarAreaDesenho(LARGURA, ALTURA);
    const wchar_t* pasta = L"aulas\\Aula_30_menu_completo\\imagens\\";
    auto caminho = [&](const wchar_t* nome) { return std::wstring(pasta) + nome; };

    ultimoEvento = L"(nenhum)";
    fotografar(caminho(L"menu_1_principal.png").c_str());

    // Atalho 8 sem nome: o Enter das fases manda para os nomes e lembra de voltar.
    lerTeclado('8', 0);
    conferir(telaMenu == MENU_FASES, "8 abre o seletor de fases");
    lerTeclado(VK_RETURN, 0);
    conferir(telaMenu == MENU_NOMES && telaAposNomes == MENU_FASES, "Enter sem nome vai para NOMES e lembra de voltar a FASES");
    lerTeclado(VK_RETURN, 0);
    conferir(telaMenu == MENU_NOMES && !avisoNome.empty(), "nome vazio e recusado com aviso");
    lerTeclado(VK_ESCAPE, 0);

    // Rota completa.
    lerTeclado('1', 0);
    conferir(telaMenu == MENU_NOMES && telaAposNomes == MENU_DIFICULDADE, "1 abre NOMES; depois vai para DIFICULDADE");
    digitar(L"Joao");
    lerTeclado(VK_LEFT, 0); lerTeclado(VK_LEFT, 0); lerTeclado(VK_LEFT, 0);
    digitarNomeJogador(L'\b');             // apaga o 'J' antes do cursor
    digitarNomeJogador(L'J');
    lerTeclado(VK_RIGHT, 0); lerTeclado(VK_RIGHT, 0);
    digitarNomeJogador(L'\b');             // apaga o 'a'
    digitarNomeJogador(L'ã');         // e põe 'ã' (chega pronto por WM_CHAR)
    conferir(nomeJogador == L"João", "cursor, Backspace e acento editam o nome (Joao -> Joao com til)");
    relogio = 0;                           // cursor visível na foto
    ultimoEvento = L"WM_CHAR 'ã'";
    fotografar(caminho(L"menu_2_nomes.png").c_str());
    lerTeclado(VK_RETURN, (1 << 30));
    conferir(telaMenu == MENU_NOMES, "Enter de repeticao automatica (bit 30) e ignorado");
    lerTeclado(VK_RETURN, 0);
    conferir(telaMenu == MENU_DIFICULDADE && nomesProntos(), "Enter confirma o nome e segue");

    lerTeclado(VK_RIGHT, 0);
    conferir(dificuldade == 1, "seta direita escolhe MEDIO");
    ultimoEvento = L"WM_KEYDOWN codigo 39";
    fotografar(caminho(L"menu_3_dificuldade.png").c_str());
    ajustarAreaDesenho(LARGURA * 2, ALTURA * 2);   // janela com o dobro do tamanho
    clicar(margemX + px(40 + 2 * 230 + 100), margemY + px(240));
    conferir(dificuldade == 2 && telaMenu == MENU_FASES, "clique no 3o cartao com escala 2 escolhe DIFICIL");
    ajustarAreaDesenho(LARGURA, ALTURA);

    lerTeclado('0', 0);
    conferir(faseSelecionada == 10, "0 seleciona a fase 10");
    lerTeclado(VK_RIGHT, 0);
    conferir(faseSelecionada == 1, "direita na fase 10 volta para a 1");
    lerTeclado(VK_DOWN, 0);
    conferir(faseSelecionada == 6, "baixo na fase 1 vai para a 6");
    ultimoEvento = L"joystick: direcional";
    fotografar(caminho(L"menu_4_fases.png").c_str());
    lerTeclado(VK_RETURN, 0);
    conferir(telaMenu == PARTIDA, "Enter com nome pronto inicia a partida");

    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    std::printf("%s\n", falhas ? "Aula 30: HA FALHAS" : "Aula 30: menu verificado");
    return falhas ? 1 : 0;
}
