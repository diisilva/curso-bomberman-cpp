#pragma once // Evita incluir este apoio mais de uma vez no mesmo programa.
#include <windows.h> // Fornece janela, mensagens e timer.
#include <gdiplus.h> // Fornece desenho de formas e textos.
#include <algorithm> // Limita valores ao intervalo permitido.
#include <string> // Guarda título e mensagens da visualização.
#include <vector> // Localiza o codificador PNG das capturas.
#include <memory> // Mantem os PNGs reais ate o fim da janela.

namespace visual { // Agrupa apenas o apoio gráfico usado depois da aula 05.
using Cena = void (*)(Gdiplus::Graphics&, int, int); // Cena recebe quadro e variante.
inline Cena cena = nullptr; // Guarda a função de desenho definida pela aula.
inline int passo = 0; // Seleciona o quadro explicativo atual.
inline int variante = 0; // Seleciona o cenário 1 ou 2.
inline bool animando = false; // Liga a passagem automática entre quadros.
inline ULONG_PTR token = 0; // Identifica a sessão GDI+.
inline std::wstring titulo; // Guarda o nome exibido na janela.
inline std::wstring rodape;
inline bool (*teclaExtra)(WPARAM) = nullptr;
inline constexpr int LARGURA = 960; // Define largura útil em pixels.
inline constexpr int ALTURA = 640; // Define altura útil em pixels.
inline constexpr int TOTAL_PASSOS = 4; // Cada cena explica quatro momentos.
inline std::unique_ptr<Gdiplus::Bitmap> spriteJogador, spriteBomba, spriteChama, spriteTerreno, spriteItens;

inline std::wstring pastaAssets() { // Funciona ao iniciar pela raiz, build ou pasta da aula.
    const wchar_t* candidatas[] = {L"projeto_final\\assets\\",L"..\\projeto_final\\assets\\",L"..\\..\\projeto_final\\assets\\"};
    for (const wchar_t* pasta : candidatas)
        if (GetFileAttributesW(pasta) != INVALID_FILE_ATTRIBUTES) return pasta;
    return candidatas[0];
}
inline void carregarAssets() { // Carrega uma vez para nao reabrir PNG a cada WM_PAINT.
    if (spriteJogador) return;
    std::wstring pasta = pastaAssets();
    spriteJogador = std::make_unique<Gdiplus::Bitmap>((pasta+L"personagem_branco.png").c_str());
    spriteBomba = std::make_unique<Gdiplus::Bitmap>((pasta+L"bomba_animada.png").c_str());
    spriteChama = std::make_unique<Gdiplus::Bitmap>((pasta+L"explosao_animada.png").c_str());
    spriteTerreno = std::make_unique<Gdiplus::Bitmap>((pasta+L"arenas_estudo.png").c_str());
    spriteItens = std::make_unique<Gdiplus::Bitmap>((pasta+L"itens_especiais.png").c_str());
}
inline void liberarAssets() { // Bitmaps precisam morrer antes de GdiplusShutdown.
    spriteJogador.reset(); spriteBomba.reset(); spriteChama.reset(); spriteTerreno.reset(); spriteItens.reset();
}

inline void texto(Gdiplus::Graphics& g, const std::wstring& conteudo, int x, int y,
                  int tamanho = 18, Gdiplus::Color cor = Gdiplus::Color(255,235,240,250)) { // Escreve uma legenda visível.
    Gdiplus::FontFamily familia(L"Consolas"); // Usa letras monoespaçadas para alinhar números.
    Gdiplus::Font fonte(&familia, Gdiplus::REAL(tamanho), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel); // Define tamanho em pixels.
    Gdiplus::SolidBrush pincel(cor); // Cria o pincel de texto.
    g.DrawString(conteudo.c_str(), -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(x), Gdiplus::REAL(y)), &pincel); // Mostra o texto.
} // Encerra a escrita.

inline void mostrarItens(Gdiplus::Graphics& g) { // Mesma ordem de TipoItem e desenharItem no jogo final.
    carregarAssets();
    if (!spriteItens || spriteItens->GetLastStatus()!=Gdiplus::Ok) return;
    const wchar_t* nomes[]={L"CHUTE",L"TELEPORTE",L"RAIO",L"BOMBA",L"VIDA",L"RELOGIO",L"IMUNE",L"ATRAVESSA"};
    for (int i=0;i<8;++i) {
        g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        g.DrawImage(spriteItens.get(),Gdiplus::Rect(62+i*108,106,40,40),i*32,0,32,32,Gdiplus::UnitPixel);
        texto(g,nomes[i],52+i*108,148,12);
    }
    texto(g,L"Os oito ícones de itens_especiais.png, na ordem do enum TipoItem.",52,166,13,Gdiplus::Color(255,142,213,206));
}

inline void caixa(Gdiplus::Graphics& g, int x, int y, int largura, int altura,
                  Gdiplus::Color fundo, Gdiplus::Color borda = Gdiplus::Color(255,90,105,135)) { // Desenha um estado.
    Gdiplus::SolidBrush preenchimento(fundo); // Cria cor de fundo da caixa.
    Gdiplus::Pen contorno(borda, 2); // Cria borda que separa estados.
    g.FillRectangle(&preenchimento, x, y, largura, altura); // Preenche a área.
    g.DrawRectangle(&contorno, x, y, largura, altura); // Destaca os limites.
} // Encerra a caixa.

inline void linha(Gdiplus::Graphics& g, int x1, int y1, int x2, int y2,
                  Gdiplus::Color cor, int espessura = 3) { // Liga valores ou posições.
    Gdiplus::Pen pincel(cor, Gdiplus::REAL(espessura)); // Prepara cor e espessura.
    g.DrawLine(&pincel, x1, y1, x2, y2); // Desenha o segmento.
} // Encerra a linha.

inline void seta(Gdiplus::Graphics& g, int x1, int y1, int x2, int y2,
                 Gdiplus::Color cor = Gdiplus::Color(255,255,210,60)) { // Mostra causa e efeito.
    linha(g, x1, y1, x2, y2, cor, 4); // Desenha o corpo da seta.
    int direcao = x2 >= x1 ? 1 : -1; // Decide para que lado aponta.
    linha(g, x2, y2, x2 - direcao * 12, y2 - 8, cor, 4); // Desenha ponta superior.
    linha(g, x2, y2, x2 - direcao * 12, y2 + 8, cor, 4); // Desenha ponta inferior.
} // Encerra a seta.

inline void base(Gdiplus::Graphics& g, const std::wstring& subtitulo) { // Desenha contexto comum da aula.
    g.Clear(Gdiplus::Color(255,22,28,42)); // Pinta o fundo completo antes de cada quadro.
    texto(g, titulo, 24, 14, 26, Gdiplus::Color(255,255,225,100)); // Escreve o objetivo principal.
    texto(g, subtitulo, 24, 54, 16, Gdiplus::Color(255,177,190,213)); // Nomeia a pergunta deste quadro.
    caixa(g, 20, 600, 920, 30, Gdiplus::Color(255,35,44,65)); // Cria rodapé de controle.
    texto(g, rodape.empty()?L"Setas: etapa 1–4    1: resposta    2: desafio (esconde a resposta)    Espaço: animar":rodape, 30, 605, 15); // Explica interação.
    texto(g, std::to_wstring(passo + 1) + L"/4", 870, 605, 15, Gdiplus::Color(255,255,225,100)); // Indica progresso.
} // Encerra o fundo comum.

// Cada aula fornece quatro estados reais da mesma regra. O desenho comum
// mantém as legendas e controles iguais, enquanto o conteúdo muda por aula.
struct Etapa {
    const wchar_t* pergunta;
    const wchar_t* conta;
    const wchar_t* resultado;
    const wchar_t* explicacao;
    int foco;
    int linha;
    int coluna;
};
enum class Tipo { Fluxo, Grade, Barras };
inline void explicar(Gdiplus::Graphics& g, int passoAtual, int varianteAtual,
                     Tipo tipo, const wchar_t* const itens[4],
                     const Etapa (&etapas)[4], const char* const* mapa = nullptr,
                     int linhas = 0, int colunas = 0, const int* valores = nullptr,
                     const wchar_t* contexto = L"") {
    const Etapa& e = etapas[passoAtual];
    base(g, e.pergunta);
    if (*contexto) texto(g,contexto,42,83,15,Gdiplus::Color(255,142,213,206));
    const Gdiplus::Color normal(255,47,61,85), foco(255,95,78,32), borda(255,245,205,75);
    if (tipo == Tipo::Grade && mapa) {
        carregarAssets();
        const int lado = std::min(44, std::min(410 / std::max(1, colunas), 310 / std::max(1, linhas)));
        const int x0 = 42, y0 = 118;
        for (int l=0; l<linhas; ++l) for (int c=0; c<colunas; ++c) {
            char ch = mapa[l][c];
            Gdiplus::Color cor = ch=='#' ? Gdiplus::Color(255,93,105,122) :
                                 ch=='+' ? Gdiplus::Color(255,153,97,54) :
                                 ch=='*' ? Gdiplus::Color(255,222,101,40) :
                                 ch=='B' ? Gdiplus::Color(255,55,111,175) :
                                 ch=='F' ? Gdiplus::Color(255,52,150,95) : normal;
            bool atual = l==e.linha && c==e.coluna;
            if (spriteTerreno && spriteTerreno->GetLastStatus()==Gdiplus::Ok) {
                // Mesmo recorte de temaDaFase(4): coluna 2, linhas chao/parede/caixa.
                int origemY=ch=='#'?46:ch=='+'?92:0;
                g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
                g.DrawImage(spriteTerreno.get(),Gdiplus::Rect(x0+c*lado,y0+l*lado,lado-2,lado-2),92,origemY,46,46,Gdiplus::UnitPixel);
                Gdiplus::Pen contorno(atual?borda:Gdiplus::Color(255,70,85,105),atual?3:1);
                g.DrawRectangle(&contorno,x0+c*lado,y0+l*lado,lado-2,lado-2);
            } else caixa(g,x0+c*lado,y0+l*lado,lado-2,lado-2,atual?foco:cor,atual?borda:Gdiplus::Color(255,70,85,105));
            Gdiplus::Bitmap* sprite = ch=='S' ? spriteJogador.get() : ch=='B' ? spriteBomba.get() : ch=='*' ? spriteChama.get() : nullptr;
            if (sprite && sprite->GetLastStatus()==Gdiplus::Ok) {
                g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
                int larguraOrigem=16, alturaOrigem=ch=='S'?24:16;
                int origemX=ch=='S'?16:0;
                int larguraDestino=ch=='S'?lado*2/3:lado-10;
                int alturaDestino=ch=='S'?lado-4:lado-10;
                Gdiplus::Rect destino(x0+c*lado+(lado-larguraDestino)/2,y0+l*lado+(lado-alturaDestino)/2,larguraDestino,alturaDestino);
                g.DrawImage(sprite,destino,origemX,0,larguraOrigem,alturaOrigem,Gdiplus::UnitPixel);
            } else if (ch=='F' || ch=='o') texto(g,std::wstring(1,wchar_t(ch)),x0+c*lado+lado/3,y0+l*lado+lado/5,18);
        }
        texto(g,L"# parede   + caixa   * chama   B bomba   F fim/fuga   S início   o rota",42,455,15);
        texto(g,L"Chão, parede, caixa, personagem, bomba e chama: PNGs reais do jogo (tema da fase 4).",42,475,14,Gdiplus::Color(255,142,213,206));
        for (int i=0;i<4;++i) {
            caixa(g,530,120+i*77,380,62,i==e.foco?foco:normal,i==e.foco?borda:Gdiplus::Color(255,90,105,135));
            texto(g,itens[i],545,137+i*77,17);
        }
    } else if (tipo == Tipo::Barras && valores) {
        for (int i=0;i<4;++i) {
            int altura = std::min(240,std::max(12,valores[i] * 22));
            int x = 92+i*205;
            caixa(g,x,390-altura,110,altura,i==e.foco?Gdiplus::Color(255,227,151,50):Gdiplus::Color(255,62,145,170));
            texto(g,itens[i],x-28,410,16);
            texto(g,std::to_wstring(valores[i]),x+34,365-altura,18);
        }
        texto(g,L"Altura da barra = valor calculado; a barra amarela é a etapa atual.",58,475,16);
    } else {
        for (int i=0;i<4;++i) {
            int x=40+i*228;
            caixa(g,x,190,190,122,i==e.foco?foco:normal,i==e.foco?borda:Gdiplus::Color(255,90,105,135));
            texto(g,std::to_wstring(i+1)+L".",x+12,201,16,Gdiplus::Color(255,255,220,100));
            texto(g,itens[i],x+12,240,16);
            if (i<3) seta(g,x+192,251,x+224,251);
        }
        texto(g,L"A caixa amarela é a etapa atual; as setas mostram a ordem de execução.",43,330,15,Gdiplus::Color(255,177,190,213));
        // Resultado em destaque: é a resposta que o aluno deve prever.
        caixa(g,40,368,880,104,Gdiplus::Color(255,28,52,48),Gdiplus::Color(255,88,170,120));
        texto(g,L"Resultado desta etapa",58,378,15,Gdiplus::Color(255,142,213,206));
        texto(g,varianteAtual ? L"?" : e.resultado,58,408,30,Gdiplus::Color(255,156,225,164));
    }
    caixa(g,36,496,888,92,Gdiplus::Color(255,33,43,61));
    texto(g,varianteAtual ? L"Sua vez: calcule ou preveja o resultado antes de revelar." : e.conta,52,505,18,Gdiplus::Color(255,255,220,105));
    texto(g,varianteAtual ? L"Aperte 1 para conferir." : e.resultado,52,532,18,Gdiplus::Color(255,156,225,164));
    texto(g,e.explicacao,52,557,15);
} // Fecha a explicação visual.

inline bool salvarPng(Gdiplus::Bitmap& imagem, const wchar_t* caminho) { // Exporta um quadro para teste e roteiro.
    UINT bytes=0, quantidade=0;
    if (Gdiplus::GetImageEncodersSize(&quantidade,&bytes)!=Gdiplus::Ok || bytes==0) return false;
    std::vector<BYTE> memoria(bytes);
    auto* codificadores=reinterpret_cast<Gdiplus::ImageCodecInfo*>(memoria.data());
    if (Gdiplus::GetImageEncoders(quantidade,bytes,codificadores)!=Gdiplus::Ok) return false;
    for (UINT i=0;i<quantidade;++i)
        if (std::wstring(codificadores[i].MimeType)==L"image/png")
            return imagem.Save(caminho,&codificadores[i].Clsid,nullptr)==Gdiplus::Ok;
    return false;
}

inline LRESULT CALLBACK mensagens(HWND janela, UINT mensagem, WPARAM tecla, LPARAM dados) { // Processa a janela didática.
    if (mensagem == WM_TIMER) { // Avança automaticamente quando animação está ligada.
        if (animando) passo = (passo + 1) % TOTAL_PASSOS; // Mostra próximo momento da regra.
        InvalidateRect(janela, nullptr, FALSE); return 0; // Solicita novo desenho.
    } // Encerra timer.
    if (mensagem == WM_KEYDOWN) { // Converte teclado em controle da explicação.
        if (teclaExtra && teclaExtra(tecla)) { InvalidateRect(janela,nullptr,FALSE); return 0; }
        if (tecla == VK_RIGHT) passo = std::min(TOTAL_PASSOS - 1, passo + 1); // Avança uma etapa.
        if (tecla == VK_LEFT) passo = std::max(0, passo - 1); // Volta uma etapa.
        if (tecla == '1' || tecla == '2') { variante = int(tecla - '1'); passo = 0; } // Troca o cenário.
        if (tecla == VK_SPACE) animando = !animando; // Liga ou pausa a demonstração.
        InvalidateRect(janela, nullptr, FALSE); return 0; // Mostra imediatamente o estado novo.
    } // Encerra teclado.
    if (mensagem == WM_ERASEBKGND) return 1; // Evita limpar a tela entre quadros.
    if (mensagem == WM_PAINT) { // Prepara uma imagem completa fora da janela.
        PAINTSTRUCT pintura{}; // Guarda ciclo de pintura.
        HDC dc = BeginPaint(janela, &pintura); // Obtém a superfície visível.
        { // Limita a vida do desenho antes de EndPaint.
            Gdiplus::Bitmap buffer(LARGURA, ALTURA); // Reserva quadro pronto na memória.
            Gdiplus::Graphics desenho(&buffer); // Desenha no bitmap invisível.
            desenho.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); // Suaviza diagramas.
            if (cena) cena(desenho, passo, variante); // Chama a regra visual da aula.
            Gdiplus::Graphics tela(dc); // Conecta GDI+ ao DC da janela.
            tela.DrawImage(&buffer, 0, 0); // Apresenta quadro completo sem piscada.
        } // Libera recursos GDI+ temporários.
        EndPaint(janela, &pintura); return 0; // Encerra pintura.
    } // Encerra WM_PAINT.
    if (mensagem == WM_DESTROY) { KillTimer(janela, 1); PostQuitMessage(0); return 0; } // Fecha timer e laço.
    return DefWindowProcW(janela, mensagem, tecla, dados); // Mantém comportamento padrão.
} // Encerra callback.

inline int executar(HINSTANCE instancia, int exibir, const wchar_t* nome, Cena desenho,
                    void (*finalizar)() = nullptr,
                    bool (*tratarTecla)(WPARAM) = nullptr) { // Inicia a visualização de uma aula.
    titulo = nome; cena = desenho; teclaExtra = tratarTecla; // Guarda nome e função específicos da aula.
    Gdiplus::GdiplusStartupInput entrada; // Prepara GDI+.
    if (Gdiplus::GdiplusStartup(&token, &entrada, nullptr) != Gdiplus::Ok) return 1; // Verifica inicialização.
    WNDCLASSW classe{}; // Zera os campos da classe Win32.
    classe.lpfnWndProc = mensagens; classe.hInstance = instancia; classe.lpszClassName = L"CursoVisualizador"; // Associa eventos.
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(token); return 2; } // Trata erro de registro.
    RECT tamanho{0, 0, LARGURA, ALTURA}; // Define área útil solicitada.
    DWORD estilo = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; // Fixa tamanho do quadro explicativo.
    AdjustWindowRect(&tamanho, estilo, FALSE); // Converte área útil em tamanho externo.
    HWND janela = CreateWindowExW(0, classe.lpszClassName, nome, estilo, CW_USEDEFAULT, CW_USEDEFAULT,
                                  tamanho.right - tamanho.left, tamanho.bottom - tamanho.top,
                                  nullptr, nullptr, instancia, nullptr); // Cria janela visível.
    if (!janela) { Gdiplus::GdiplusShutdown(token); return 3; } // Trata falha de criação.
    ShowWindow(janela, exibir); // Apresenta a aula.
    SetTimer(janela, 1, 800, nullptr); // Permite avanço automático a cada 800 ms.
    MSG evento{}; // Guarda mensagem da fila.
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) { TranslateMessage(&evento); DispatchMessageW(&evento); } // Mantém janela ativa.
    if (finalizar) finalizar(); // Fecha sprites especificos da aula antes de GDI+.
    liberarAssets(); // Fecha PNGs antes da biblioteca grafica.
    Gdiplus::GdiplusShutdown(token); // Libera GDI+ depois de fechar.
    return int(evento.wParam); // Devolve código de saída.
} // Encerra o programa.
} // Encerra o namespace de apoio visual.
