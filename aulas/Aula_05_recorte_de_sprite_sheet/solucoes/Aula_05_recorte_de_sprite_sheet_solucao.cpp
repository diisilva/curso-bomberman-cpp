// =============================================================================
// Aula 05 — Recorte de sprite sheet
// -----------------------------------------------------------------------------
// Uma sprite sheet (folha de sprites) é UMA imagem com VÁRIOS desenhos. Para
// mostrar um só, recortamos um retângulo da folha (ORIGEM) e copiamos esse
// pedaço para um retângulo da janela (DESTINO):
//
//     folha.png                               janela
//     +----------------------------+          +---------------------+
//     |  [q0][q1][q2][q3] ...      |  ----->  |      +--------+     |
//     |      ^ origem (x,y,l,a)    | DrawImage|      |destino |     |
//     +----------------------------+          |      +--------+     |
//                                             +---------------------+
//
// ANIMAR = trocar a ORIGEM de tempos em tempos e pedir um novo desenho.
//
// Dois modos de animação:
//   - FOLHA INTEIRA (ao abrir): passa por TODOS os quadros, um de cada vez,
//     na ordem de leitura (linha por linha, da esquerda para a direita).
//   - DIREÇÃO (ao apertar uma seta): repete só os quadros daquela direção,
//     como acontece no jogo quando o personagem anda.
//
// O programa traz TRÊS folhas, cada uma ensinando uma ideia:
//   1) personagem_branco.png ........ grade regular: o recorte sai de uma FÓRMULA.
//   2) bomba_animada.png ............ fileira única: animação sem direção.
//   3) personagem_principal_sprite_v2 folha irregular: o recorte sai de uma TABELA
//                                     medida à mão, e o fundo roxo é removido.
//
// Controles:  1 / 2 / 3 = troca a folha     Setas = modo DIREÇÃO
//             A         = modo FOLHA INTEIRA
//             Espaço    = anda / para       T     = liga/desliga a cor-chave
//
// Solução do exercício 2: troque o intervalo de 150 para 250 ms e compare.
// Compile esta solução com g++ -std=c++17 -O2 -Wall -Wextra -pedantic.
// Execute na raiz do curso para localizar projeto_final\assets.
// =============================================================================
#include <windows.h> // Janela, mensagens e timer do Win32.
#include <gdiplus.h> // Bitmap, Graphics, DrawImage e ImageAttributes.
#include <algorithm> // std::min e std::sort.
#include <cwchar>    // swprintf, para montar textos na tela.
#include <memory>    // unique_ptr, para as imagens serem liberadas sozinhas.
#include <string>    // wstring, para montar caminhos de arquivo.
#include <vector>    // vector, para a lista de folhas e os ciclos de animação.

// -----------------------------------------------------------------------------
// 1) COMO DESCREVEMOS UMA FOLHA
// Todas as folhas viram a MESMA estrutura: uma tabela de retângulos
// quadros[direção][passo]. O resto do programa só consulta essa tabela e
// não precisa saber se ela foi calculada por fórmula ou medida à mão.
// -----------------------------------------------------------------------------
enum Direcao { CIMA = 0, ESQUERDA = 1, BAIXO = 2, DIREITA = 3 };
const wchar_t* NOMES_DIRECAO[4] = {L"CIMA", L"ESQUERDA", L"BAIXO", L"DIREITA"};
const int MAX_PASSOS = 4; // Maior número de quadros por direção entre as nossas folhas.

struct FolhaDeSprites {
    const wchar_t* titulo = L"";           // Nome mostrado na tela.
    const wchar_t* arquivo = L"";          // Nome do PNG dentro de projeto_final\assets.
    const wchar_t* explicacao = L"";       // Uma frase sobre o que essa folha ensina.
    int direcoes = 1;                      // 4 = personagem; 1 = objeto sem direção (bomba).
    int passos = 1;                        // Quadros de animação em cada direção.
    Gdiplus::Rect quadros[4][MAX_PASSOS];  // A TABELA: onde está cada quadro na folha.
    std::vector<int> ciclo;                // Ordem em que os passos são mostrados.
    int quadroParado = 0;                  // Passo exibido quando o personagem para.
    bool usaCorChave = false;              // true = a folha tem fundo colorido a remover.
    Gdiplus::Color corChave;               // Cor do fundo a remover (se usaCorChave).
    Gdiplus::Rect vista;                   // Trecho da folha mostrado no painel de cima.
    int gradeLargura = 0, gradeAltura = 0; // > 0 quando a folha é uma grade regular.
    std::vector<Gdiplus::Rect> emOrdemDeLeitura; // Todos os quadros, de cima p/ baixo, esq. p/ dir.
    std::unique_ptr<Gdiplus::Bitmap> imagem; // A imagem carregada (nula se falhou).
};

// GRADE REGULAR: todos os quadros têm o mesmo tamanho e ficam lado a lado.
// Então a posição é uma conta simples:
//     x = coluna * largura        y = linha * altura
// linhaDaDirecao diz em qual linha da folha está cada direção, porque cada
// artista organiza a folha numa ordem diferente.
void montarGrade(FolhaDeSprites& f, int largura, int altura, const int linhaDaDirecao[4]) {
    f.gradeLargura = largura;
    f.gradeAltura = altura;
    for (int d = 0; d < f.direcoes; ++d)        // Para cada direção...
        for (int p = 0; p < f.passos; ++p)      // ...e cada passo:
            f.quadros[d][p] = Gdiplus::Rect(p * largura, linhaDaDirecao[d] * altura, largura, altura);
}

// -----------------------------------------------------------------------------
// 2) AS TRÊS FOLHAS DA AULA
// Para incluir uma quarta folha, crie outra função como estas e adicione-a
// em criarFolhas(). Nada mais no programa precisa mudar.
// -----------------------------------------------------------------------------

// Folha 1: grade 3 colunas x 4 linhas, quadros de 16x24, fundo já transparente.
// Ordem das linhas nesta folha: 0 = frente (BAIXO), 1 = DIREITA, 2 = costas (CIMA), 3 = ESQUERDA.
FolhaDeSprites criarPersonagemBranco() {
    FolhaDeSprites f;
    f.titulo = L"Personagem branco";
    f.arquivo = L"personagem_branco.png";
    f.explicacao = L"Grade regular: x = coluna*16, y = linha*24.";
    f.direcoes = 4;
    f.passos = 3;
    const int linhaDaDirecao[4] = {2, 3, 0, 1}; // CIMA, ESQUERDA, BAIXO, DIREITA -> linha na folha.
    montarGrade(f, 16, 24, linhaDaDirecao);
    f.quadroParado = 1;      // Nesta folha o quadro do meio é o "parado".
    f.ciclo = {1, 0, 1, 2};  // parado -> passo A -> parado -> passo B (igual ao jogo final).
    f.vista = Gdiplus::Rect(0, 0, 48, 96); // Mostra a folha inteira.
    return f;
}

// Folha 2: uma fileira de 4 quadros 16x16. Não tem direção, só "pulsa".
FolhaDeSprites criarBomba() {
    FolhaDeSprites f;
    f.titulo = L"Bomba animada";
    f.arquivo = L"bomba_animada.png";
    f.explicacao = L"Fileira única: só o passo muda, não há direção.";
    f.direcoes = 1;
    f.passos = 4;
    const int linhaDaDirecao[4] = {0, 0, 0, 0}; // Tudo está na linha 0.
    montarGrade(f, 16, 16, linhaDaDirecao);
    f.quadroParado = 0;
    f.ciclo = {0, 1, 2, 3}; // Cresce e encolhe: a própria folha já traz a volta.
    f.vista = Gdiplus::Rect(0, 0, 64, 16);
    return f;
}

// Folha 3: folha "de verdade", copiada de um jogo. Tem título, margens e
// quadros de 17 ou 18 pixels de largura. Não há fórmula que acerte todos,
// então medimos cada quadro na imagem e escrevemos a tabela à mão.
// Os quadros usados estão na primeira fileira (Y = 38, altura 26).
FolhaDeSprites criarPersonagemFolhaIrregular() {
    FolhaDeSprites f;
    f.titulo = L"Bomberman (folha irregular)";
    f.arquivo = L"personagem_principal_sprite_v2.png";
    f.explicacao = L"Folha irregular: posições medidas à mão + cor-chave.";
    f.direcoes = 4;
    f.passos = 3;
    const int y = 38, altura = 26;
    const int x[4][3] = {      // X de cada quadro, medido na imagem.
        { 18,  36,  54},       // CIMA (de costas)
        { 73,  92, 111},       // ESQUERDA
        {131, 149, 167},       // BAIXO (de frente)
        {186, 205, 224},       // DIREITA
    };
    const int largura[4] = {17, 18, 17, 18}; // Largura dos quadros em cada direção.
    for (int d = 0; d < 4; ++d)
        for (int p = 0; p < 3; ++p)
            f.quadros[d][p] = Gdiplus::Rect(x[d][p], y, largura[d], altura);
    f.quadroParado = 0;     // Aqui o primeiro quadro é o "parado".
    f.ciclo = {0, 1, 0, 2};
    f.usaCorChave = true;   // Os quadros têm fundo roxo...
    f.corChave = Gdiplus::Color(255, 112, 0, 170); // ...exatamente esta cor.
    f.vista = Gdiplus::Rect(14, 30, 232, 40); // Mostra só a primeira fileira.
    return f;
}

// -----------------------------------------------------------------------------
// 3) ESTADO DO PROGRAMA
// -----------------------------------------------------------------------------
ULONG_PTR token = 0;                 // Identifica a sessão GDI+.
std::vector<FolhaDeSprites> folhas;  // As folhas disponíveis.
size_t folhaAtual = 0;               // Qual delas está na tela.
Direcao direcao = BAIXO;             // Direção escolhida pelas setas.
size_t passoDoCiclo = 0;             // Posição dentro de folha.ciclo.
bool modoFolhaInteira = true;        // true = percorre todos os quadros; false = só uma direção.
size_t indiceNaFolha = 0;            // Posição dentro de folha.emOrdemDeLeitura.
bool andando = true;                 // Se false, a animação congela.
bool corChaveLigada = true;          // Liga/desliga a remoção do fundo.

const int LARGURA_JANELA = 900, ALTURA_JANELA = 600;
const UINT_PTR ID_TIMER = 1;   // Identificador do nosso relógio.
const UINT INTERVALO_MS = 250; // Quatro mudanças de quadro por segundo para comparação.
const int ESCALA_SPRITE = 8;   // O quadro recortado aparece 8x maior.

// Procura a pasta de assets do projeto final a partir de onde o .exe foi
// executado: raiz do curso, build\ ou a pasta da aula.
std::wstring pastaDeAssets() {
    const wchar_t* candidatas[] = {
        L"projeto_final\\assets\\",          // Executado da raiz do curso.
        L"..\\projeto_final\\assets\\",      // Executado de build\.
        L"..\\..\\projeto_final\\assets\\",  // Executado da pasta da aula.
    };
    for (const wchar_t* pasta : candidatas)
        if (GetFileAttributesW(pasta) != INVALID_FILE_ATTRIBUTES) return pasta; // A pasta existe.
    return candidatas[0]; // Não achou: devolve a padrão (o desenho mostrará o erro).
}

void criarFolhas() {
    folhas.push_back(criarPersonagemBranco());
    folhas.push_back(criarBomba());
    folhas.push_back(criarPersonagemFolhaIrregular());
    std::wstring pasta = pastaDeAssets();
    for (FolhaDeSprites& f : folhas) {
        f.imagem = std::make_unique<Gdiplus::Bitmap>((pasta + f.arquivo).c_str()); // Carrega o PNG.
        if (f.imagem->GetLastStatus() != Gdiplus::Ok) f.imagem.reset();           // Falhou: deixa nulo.

        // Junta todos os quadros da tabela numa lista só e ordena como lemos
        // um texto: primeiro pela linha (Y), depois pela coluna (X).
        for (int d = 0; d < f.direcoes; ++d)
            for (int p = 0; p < f.passos; ++p) f.emOrdemDeLeitura.push_back(f.quadros[d][p]);
        std::sort(f.emOrdemDeLeitura.begin(), f.emOrdemDeLeitura.end(),
                  [](const Gdiplus::Rect& a, const Gdiplus::Rect& b) {
                      return a.Y != b.Y ? a.Y < b.Y : a.X < b.X;
                  });
    }
}

// Qual passo (coluna da tabela) deve aparecer agora?
int passoAtual(const FolhaDeSprites& f) {
    return andando ? f.ciclo[passoDoCiclo % f.ciclo.size()] : f.quadroParado;
}

// Folhas sem direção usam sempre a linha 0 da tabela.
int direcaoNaTabela(const FolhaDeSprites& f) {
    return f.direcoes == 1 ? 0 : direcao;
}

// -----------------------------------------------------------------------------
// 4) DESENHO
// -----------------------------------------------------------------------------
void escrever(Gdiplus::Graphics& g, const wchar_t* texto, float x, float y, float tamanho, Gdiplus::Color cor) {
    Gdiplus::FontFamily familia(L"Consolas"); // Fonte monoespaçada: números alinhados.
    Gdiplus::Font fonte(&familia, tamanho, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(texto, -1, &fonte, Gdiplus::PointF(x, y), &pincel);
}

// Copia o retângulo "origem" da folha para o retângulo "destino" da tela.
// É AQUI que o recorte acontece; todo o resto da aula prepara estes números.
void desenharRecorte(Gdiplus::Graphics& g, const FolhaDeSprites& f, const Gdiplus::Rect& origem,
                     const Gdiplus::Rect& destino, bool removerFundo) {
    // ImageAttributes muda COMO a imagem é copiada. SetColorKey(a, b) torna
    // transparente toda cor entre a e b; aqui, só a cor exata do fundo.
    Gdiplus::ImageAttributes atributos;
    if (removerFundo && f.usaCorChave) atributos.SetColorKey(f.corChave, f.corChave);
    g.DrawImage(f.imagem.get(), destino,                         // Para onde vai.
                origem.X, origem.Y, origem.Width, origem.Height, // De onde vem.
                Gdiplus::UnitPixel, &atributos);                 // Unidade e efeitos.
}

void desenharCena(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 24, 28, 40)); // Fundo escuro da janela.

    // Pixel art ampliada precisa destes dois ajustes:
    // - NearestNeighbor: cada pixel vira um bloco, sem borrar.
    // - PixelOffsetModeHalf: alinha os blocos, sem cortar meia borda.
    g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    const FolhaDeSprites& f = folhas[folhaAtual];
    Gdiplus::Color branco(255, 255, 255, 255), cinza(255, 150, 160, 180), amarelo(255, 255, 220, 0);
    wchar_t linha[160];

    swprintf(linha, 160, L"[%zu/%zu] %ls  -  %ls", folhaAtual + 1, folhas.size(), f.titulo, f.arquivo);
    escrever(g, linha, 20, 10, 18, branco);
    escrever(g, f.explicacao, 20, 34, 15, cinza);

    if (!f.imagem) { // Sem imagem não há o que recortar.
        escrever(g, L"Imagem não encontrada em projeto_final\\assets.", 20, 80, 16, Gdiplus::Color(255, 255, 120, 120));
        escrever(g, L"Execute o programa a partir da raiz do curso.", 20, 104, 16, branco);
        return;
    }

    // ---- Painel de cima: a FOLHA, com todos os quadros da tabela marcados ----
    // A escala é escolhida para o trecho caber na caixa de 860 x 290 pixels.
    const int caixaX = 20, caixaY = 62, caixaL = 860, caixaA = 290;
    int escalaFolha = std::min(6, std::min(caixaL / f.vista.Width, caixaA / f.vista.Height));
    Gdiplus::Rect vistaNaTela(caixaX, caixaY, f.vista.Width * escalaFolha, f.vista.Height * escalaFolha);
    Gdiplus::SolidBrush fundoVista(Gdiplus::Color(255, 60, 64, 90)); // Destaca as áreas transparentes.
    g.FillRectangle(&fundoVista, vistaNaTela);
    desenharRecorte(g, f, f.vista, vistaNaTela, false); // A folha aparece crua, sem cor-chave.

    // Converte um retângulo da FOLHA para a posição dele na TELA.
    auto naTela = [&](const Gdiplus::Rect& r) {
        return Gdiplus::Rect(caixaX + (r.X - f.vista.X) * escalaFolha, caixaY + (r.Y - f.vista.Y) * escalaFolha,
                             r.Width * escalaFolha, r.Height * escalaFolha);
    };
    Gdiplus::Pen contorno(Gdiplus::Color(120, 255, 255, 255), 1.0f);
    for (int d = 0; d < f.direcoes; ++d)          // Contorno fino em todos os quadros conhecidos...
        for (int p = 0; p < f.passos; ++p) g.DrawRectangle(&contorno, naTela(f.quadros[d][p]));

    int d = direcaoNaTabela(f), p = passoAtual(f);
    Gdiplus::Rect origem = modoFolhaInteira      // >>> O RECORTE DESTE INSTANTE <<<
        ? f.emOrdemDeLeitura[indiceNaFolha % f.emOrdemDeLeitura.size()] // Próximo da folha inteira...
        : f.quadros[d][p];                                              // ...ou da direção escolhida.
    Gdiplus::Pen destaque(amarelo, 3.0f);
    g.DrawRectangle(&destaque, naTela(origem)); // ...e contorno amarelo no quadro atual.

    // ---- Painel de baixo: o recorte AMPLIADO no destino ----------------------
    // Se o destino for maior que a origem, a imagem é ampliada.
    Gdiplus::Rect destino(40, 372, origem.Width * ESCALA_SPRITE, origem.Height * ESCALA_SPRITE);
    desenharRecorte(g, f, origem, destino, corChaveLigada);

    // ---- Painel de texto: os números que mudam a cada quadro -----------------
    float tx = 240, ty = 374;
    if (modoFolhaInteira) {
        swprintf(linha, 160, L"Modo    : FOLHA INTEIRA (setas = só uma direção)");
        escrever(g, linha, tx, ty, 17, branco);
        swprintf(linha, 160, L"Quadro  : %zu de %zu", indiceNaFolha % f.emOrdemDeLeitura.size() + 1,
                 f.emOrdemDeLeitura.size());
    } else {
        if (f.direcoes == 1) swprintf(linha, 160, L"Modo    : DIREÇÃO (esta folha não tem direção)");
        else                 swprintf(linha, 160, L"Modo    : DIREÇÃO %ls (A = folha inteira)", NOMES_DIRECAO[d]);
        escrever(g, linha, tx, ty, 17, branco);
        if (andando) swprintf(linha, 160, L"Passo   : ciclo[%zu] = quadro %d", passoDoCiclo % f.ciclo.size(), p);
        else         swprintf(linha, 160, L"Passo   : parado = quadro %d", p);
    }
    escrever(g, linha, tx, ty + 26, 17, branco);
    swprintf(linha, 160, L"origem  : Rect(%d, %d, %d, %d)", origem.X, origem.Y, origem.Width, origem.Height);
    escrever(g, linha, tx, ty + 52, 17, amarelo);
    if (f.gradeLargura > 0) { // Em grade, mostramos a conta que gerou a origem.
        swprintf(linha, 160, L"          x = %d * %d   y = %d * %d", origem.X / f.gradeLargura, f.gradeLargura,
                 origem.Y / f.gradeAltura, f.gradeAltura);
        escrever(g, linha, tx, ty + 74, 15, amarelo);
    } else {
        escrever(g, L"          (valores lidos da tabela medida)", tx, ty + 74, 15, amarelo);
    }
    swprintf(linha, 160, L"destino : Rect(%d, %d, %d, %d)  (%dx maior)", destino.X, destino.Y, destino.Width,
             destino.Height, ESCALA_SPRITE);
    escrever(g, linha, tx, ty + 100, 17, branco);
    if (f.usaCorChave)
        swprintf(linha, 160, L"Fundo   : cor-chave RGB(%d,%d,%d) %ls", f.corChave.GetR(), f.corChave.GetG(),
                 f.corChave.GetB(), corChaveLigada ? L"removida" : L"visível");
    else
        swprintf(linha, 160, L"Fundo   : transparente no próprio PNG (canal alfa)");
    escrever(g, linha, tx, ty + 126, 17, branco);
    escrever(g, L"1/2/3: folha   Setas: direção   A: folha inteira   Espaço: anda/para   T: cor-chave",
             20, ty + 196, 14, cinza);
}

// -----------------------------------------------------------------------------
// 5) MENSAGENS DO WINDOWS
// -----------------------------------------------------------------------------
LRESULT CALLBACK janelaProc(HWND janela, UINT mensagem, WPARAM w, LPARAM l) {
    switch (mensagem) {
    case WM_TIMER: // O relógio bateu: hora de avançar a animação.
        if (andando) {
            passoDoCiclo = (passoDoCiclo + 1) % folhas[folhaAtual].ciclo.size();                // Modo direção.
            indiceNaFolha = (indiceNaFolha + 1) % folhas[folhaAtual].emOrdemDeLeitura.size();   // Modo folha inteira.
        }
        InvalidateRect(janela, nullptr, FALSE); // Pede um novo WM_PAINT.
        return 0;

    case WM_KEYDOWN: // Uma tecla foi pressionada.
        if (w >= '1' && w < '1' + folhas.size()) { folhaAtual = w - '1'; passoDoCiclo = indiceNaFolha = 0; }
        if (w == VK_UP)    { direcao = CIMA;     modoFolhaInteira = false; }
        if (w == VK_LEFT)  { direcao = ESQUERDA; modoFolhaInteira = false; }
        if (w == VK_DOWN)  { direcao = BAIXO;    modoFolhaInteira = false; }
        if (w == VK_RIGHT) { direcao = DIREITA;  modoFolhaInteira = false; }
        if (w == 'A')      { modoFolhaInteira = true; indiceNaFolha = 0; }
        if (w == VK_SPACE) andando = !andando;
        if (w == 'T')      corChaveLigada = !corChaveLigada;
        InvalidateRect(janela, nullptr, FALSE); // Redesenha já, sem esperar o timer.
        return 0;

    case WM_ERASEBKGND: // Não deixa o Windows apagar a janela antes de pintar:
        return 1;       // nós pintamos tudo, então isso só causaria piscadas.

    case WM_PAINT: {
        PAINTSTRUCT pintura{};
        HDC dc = BeginPaint(janela, &pintura);
        RECT area;
        GetClientRect(janela, &area);

        // DOUBLE BUFFERING: desenhamos primeiro numa imagem invisível na
        // memória e só no fim copiamos tudo de uma vez para a tela.
        // Assim ninguém vê o desenho "pela metade" (a janela não pisca).
        Gdiplus::Bitmap buffer(area.right, area.bottom);
        Gdiplus::Graphics desenhoNoBuffer(&buffer);
        desenharCena(desenhoNoBuffer);

        Gdiplus::Graphics tela(dc);
        tela.DrawImage(&buffer, 0, 0); // Copia o quadro pronto para a janela.
        EndPaint(janela, &pintura);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(janela, ID_TIMER); // Desliga o relógio.
        PostQuitMessage(0);          // Encerra o laço de mensagens.
        return 0;
    }
    return DefWindowProcW(janela, mensagem, w, l); // Demais mensagens: comportamento padrão.
}

// -----------------------------------------------------------------------------
// 6) PONTO DE ENTRADA
// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&token, &entrada, nullptr) != Gdiplus::Ok) return 1; // Liga o GDI+.
    criarFolhas();

    WNDCLASSW classe{};
    classe.lpfnWndProc = janelaProc;
    classe.hInstance = instancia;
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    classe.lpszClassName = L"AulaSprites";
    if (!RegisterClassW(&classe)) { folhas.clear(); Gdiplus::GdiplusShutdown(token); return 2; }

    // Ajusta o tamanho externo para que a ÁREA ÚTIL tenha LARGURA x ALTURA.
    RECT tamanho{0, 0, LARGURA_JANELA, ALTURA_JANELA};
    DWORD estilo = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; // Janela de tamanho fixo.
    AdjustWindowRect(&tamanho, estilo, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 05 - Recorte de sprite sheet", estilo,
                                  CW_USEDEFAULT, CW_USEDEFAULT, tamanho.right - tamanho.left,
                                  tamanho.bottom - tamanho.top, nullptr, nullptr, instancia, nullptr);
    if (!janela) { folhas.clear(); Gdiplus::GdiplusShutdown(token); return 3; }

    ShowWindow(janela, exibir);
    SetTimer(janela, ID_TIMER, INTERVALO_MS, nullptr); // Gera um WM_TIMER a cada INTERVALO_MS.

    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) { // Laço de mensagens.
        TranslateMessage(&evento);
        DispatchMessageW(&evento);
    }

    folhas.clear();                  // Libera as imagens ANTES de desligar o GDI+.
    Gdiplus::GdiplusShutdown(token); // Desliga o GDI+.
    return 0;
}
