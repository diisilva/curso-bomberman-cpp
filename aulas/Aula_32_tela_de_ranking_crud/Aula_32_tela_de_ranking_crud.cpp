// Aula 32 — A tela de ranking: listar, renomear e excluir
//
// A tela de ranking do jogo real (opção 6 do menu) em tamanho de aula, com as
// mesmas funções de projeto_final/bomberman.cpp:
//   - um registro por nome: chaveNomeRanking ignora maiúsculas e espaços repetidos
//   - Read:   lerRanking relê o CSV, descarta linhas ruins e ordena (melhorRegistro)
//   - Update: editarNomeRanking troca só o nome; a pontuação fica
//   - Delete: excluirRegistroRanking apaga pela chave, depois de uma confirmação
//   - Create: salvarRegistroPartida (tecla N simula o fim de uma partida)
//   - toda gravação passa por gravarRanking: .bak, .tmp e MoveFileExA (Aula 21)
// O arquivo é ranking_aula32.csv, na pasta em que o programa roda. Abra-o no
// Bloco de Notas, altere, salve e aperte R para ver a tela reler.
#include <windows.h>
#include <gdiplus.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

ULONG_PTR tokenGdiPlus = 0;
const int LARGURA = 760, ALTURA = 520;     // coordenadas de projeto da tela
const int POR_PAGINA = 10;
std::string caminhoRanking = "ranking_aula32.csv";

// ---------- texto: UTF-8 no arquivo, UTF-16 na tela (como wparaStr/strParaW) ----------
std::string wparaStr(const std::wstring& texto) {
    int n = WideCharToMultiByte(CP_UTF8, 0, texto.data(), int(texto.size()), nullptr, 0, nullptr, nullptr);
    std::string resultado(n, '\0');
    if (n) WideCharToMultiByte(CP_UTF8, 0, texto.data(), int(texto.size()), resultado.data(), n, nullptr, nullptr);
    return resultado;
}
std::wstring strParaW(const std::string& texto) {
    int n = MultiByteToWideChar(CP_UTF8, 0, texto.data(), int(texto.size()), nullptr, 0);
    std::wstring resultado(n, L'\0');
    if (n) MultiByteToWideChar(CP_UTF8, 0, texto.data(), int(texto.size()), resultado.data(), n);
    return resultado;
}

// ---------- o registro e a identidade de um nome ----------
struct RegistroPontuacao {
    std::string data, jogador;
    int pontuacao = 0, bombas = 0, movimentos = 0;
};

// "  Ana   Lu " vira "Ana Lu": sem espaços nas pontas nem repetidos.
std::string limparNomeRanking(const std::string& nome) {
    std::string limpo;
    bool espaco = false;
    for (unsigned char c : nome) {
        if (c == ' ') { espaco = !limpo.empty(); continue; }
        if (c < 32) continue;
        if (espaco) { limpo += ' '; espaco = false; }
        limpo += char(c);
    }
    return limpo;
}
// A CHAVE decide se dois nomes são a mesma pessoa: "ANA", "ana" e " Ana " coincidem.
std::string chaveNomeRanking(const std::string& nome) {
    std::wstring chave = strParaW(limparNomeRanking(nome)), minuscula(chave.size(), L'\0');
    if (!chave.empty())
        LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, chave.data(), int(chave.size()),
                      minuscula.data(), int(minuscula.size()), nullptr, nullptr, 0);
    return wparaStr(minuscula);
}
bool mesmoNome(const std::string& a, const std::string& b) { return chaveNomeRanking(a) == chaveNomeRanking(b); }

bool melhorRegistro(const RegistroPontuacao& a, const RegistroPontuacao& b) {
    if (a.pontuacao != b.pontuacao) return a.pontuacao > b.pontuacao;
    if (a.bombas != b.bombas) return a.bombas < b.bombas;
    if (a.movimentos != b.movimentos) return a.movimentos < b.movimentos;
    return chaveNomeRanking(a.jogador) < chaveNomeRanking(b.jogador);
}

// ---------- CSV (Aula 21) ----------
std::string campoCsv(const std::string& campo) {
    if (campo.find_first_of(";\"\r\n") == std::string::npos) return campo;
    std::string saida = "\"";
    for (char c : campo) { if (c == '"') saida += '"'; saida += c; }
    return saida + '"';
}
bool lerCamposCsv(const std::string& linha, std::vector<std::string>& campos) {
    campos.clear();
    std::string campo;
    bool aspas = false, fechado = false;
    for (size_t i = 0; i < linha.size(); ++i) {
        char c = linha[i];
        if (aspas) {
            if (c != '"') campo += c;
            else if (i + 1 < linha.size() && linha[i + 1] == '"') { campo += '"'; ++i; }
            else { aspas = false; fechado = true; }
        } else if (c == ';') { campos.push_back(campo); campo.clear(); fechado = false; }
        else if (c == '"') { if (!campo.empty() || fechado) return false; aspas = true; }
        else if (c != '\r') { if (fechado) return false; campo += c; }
    }
    if (aspas) return false;
    campos.push_back(campo);
    return campos.size() == 5;
}
bool inteiroValido(const std::string& valor, int& destino) {
    try { size_t fim; destino = std::stoi(valor, &fim); return fim == valor.size() && destino >= 0; }
    catch (...) { return false; }
}

// ---------- R: ler (relê sempre do arquivo, como o jogo) ----------
std::vector<RegistroPontuacao> lerRanking() {
    std::vector<RegistroPontuacao> registros;
    std::ifstream arquivo(caminhoRanking);
    std::string linha;
    std::vector<std::string> campos;
    while (std::getline(arquivo, linha)) {
        if (linha.empty() || !lerCamposCsv(linha, campos)) continue;
        RegistroPontuacao r;
        r.data = campos[0];
        r.jogador = limparNomeRanking(campos[1]);
        if (r.jogador.empty() || !inteiroValido(campos[2], r.pontuacao) ||
            !inteiroValido(campos[3], r.bombas) || !inteiroValido(campos[4], r.movimentos)) continue;
        // Arquivo editado à mão com o mesmo nome duas vezes: fica o melhor.
        auto existente = std::find_if(registros.begin(), registros.end(),
                                      [&](const RegistroPontuacao& x) { return mesmoNome(x.jogador, r.jogador); });
        if (existente == registros.end()) registros.push_back(r);
        else if (melhorRegistro(r, *existente)) *existente = r;
    }
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    return registros;
}

// ---------- toda escrita passa por aqui (Aula 21) ----------
bool gravarRanking(std::vector<RegistroPontuacao> registros) {
    std::sort(registros.begin(), registros.end(), melhorRegistro);
    CopyFileA(caminhoRanking.c_str(), (caminhoRanking + ".bak").c_str(), TRUE);   // só a 1ª versão
    std::string temporario = caminhoRanking + ".tmp";
    std::ofstream arquivo(temporario, std::ios::trunc);
    if (!arquivo) return false;
    for (const auto& r : registros)
        arquivo << campoCsv(r.data) << ';' << campoCsv(r.jogador) << ';' << r.pontuacao << ';'
                << r.bombas << ';' << r.movimentos << '\n';
    arquivo.flush();
    if (!arquivo) return false;
    arquivo.close();
    return MoveFileExA(temporario.c_str(), caminhoRanking.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
}

// ---------- C: fim de partida — um registro por nome, só o melhor fica ----------
bool salvarRegistroPartida(RegistroPontuacao novo) {
    novo.jogador = limparNomeRanking(novo.jogador);
    if (novo.jogador.empty()) return false;
    auto registros = lerRanking();
    auto existente = std::find_if(registros.begin(), registros.end(),
                                  [&](const RegistroPontuacao& r) { return mesmoNome(r.jogador, novo.jogador); });
    if (existente == registros.end()) registros.push_back(novo);
    else if (melhorRegistro(novo, *existente)) *existente = novo;
    else return true;   // pior que o recorde: nada muda (o jogo regrava igual; aqui pulamos)
    return gravarRanking(registros);
}

// ---------- U: renomear sem perder pontos e sem colidir com outro nome ----------
bool editarNomeRanking(const std::string& antigo, const std::string& novo) {
    auto registros = lerRanking();
    std::string limpo = limparNomeRanking(novo);
    if (limpo.empty() || strParaW(limpo).size() > 24) return false;
    auto alvo = std::find_if(registros.begin(), registros.end(),
                             [&](const RegistroPontuacao& r) { return mesmoNome(r.jogador, antigo); });
    if (alvo == registros.end()) return false;
    for (const auto& r : registros)
        if (&r != &*alvo && mesmoNome(r.jogador, limpo)) return false;   // "ana" não pode virar "Bia" se Bia existe
    alvo->jogador = limpo;
    return gravarRanking(registros);
}

// ---------- D: excluir pela chave ----------
bool excluirRegistroRanking(const std::string& nome) {
    auto registros = lerRanking();
    size_t antes = registros.size();
    registros.erase(std::remove_if(registros.begin(), registros.end(),
                                   [&](const RegistroPontuacao& r) { return mesmoNome(r.jogador, nome); }),
                    registros.end());
    return registros.size() < antes && gravarRanking(registros);
}

// ---------- estado da tela ----------
int rankingSelecionado = 0;
bool editandoNome = false;           // campo de renomear aberto
bool confirmandoExclusao = false;    // o jogo usa MessageBoxW; aqui a pergunta é desenhada na tela
std::wstring nomeEmEdicao;
size_t cursorNome = 0;
std::string nomeOriginalRanking;     // quem está sendo renomeado (o nome pode mudar enquanto digita)
std::wstring avisoRanking;
ULONGLONG relogio = 0;
unsigned sementePartida = 7;

int totalRegistros() { return int(lerRanking().size()); }
void limitarSelecao() { rankingSelecionado = std::clamp(rankingSelecionado, 0, std::max(0, totalRegistros() - 1)); }

void iniciarRenomeacaoRanking() {
    auto registros = lerRanking();
    if (registros.empty()) return;
    limitarSelecao();
    nomeOriginalRanking = registros[rankingSelecionado].jogador;
    nomeEmEdicao = strParaW(nomeOriginalRanking);
    cursorNome = nomeEmEdicao.size();
    editandoNome = true;
    avisoRanking.clear();
}
void terminarRenomeacaoRanking(bool confirmar) {
    if (confirmar && !editarNomeRanking(nomeOriginalRanking, wparaStr(nomeEmEdicao))) {
        avisoRanking = L"Nome vazio, repetido ou arquivo sem permissao. Escolha outro nome.";
        return;   // o campo continua aberto para corrigir
    }
    if (confirmar) {
        // A ordem não muda (pontos iguais), mas o desempate final é o nome: reencontre a linha.
        auto registros = lerRanking();
        std::string novo = limparNomeRanking(wparaStr(nomeEmEdicao));
        for (int i = 0; i < int(registros.size()); ++i)
            if (mesmoNome(registros[i].jogador, novo)) rankingSelecionado = i;
    }
    editandoNome = false;
    nomeOriginalRanking.clear();
    avisoRanking = confirmar ? L"Nome atualizado. Pontuacao preservada." : L"Edicao cancelada.";
}
void pedirExclusaoRanking() {
    if (totalRegistros() == 0) return;
    limitarSelecao();
    confirmandoExclusao = true;
    avisoRanking.clear();
}
void responderExclusao(bool sim) {
    confirmandoExclusao = false;
    if (!sim) { avisoRanking = L"Exclusao cancelada."; return; }
    auto registros = lerRanking();
    bool ok = excluirRegistroRanking(registros[rankingSelecionado].jogador);
    avisoRanking = ok ? L"Registro excluido. Esse nome pode recomecar do zero." : L"Nao foi possivel excluir o registro.";
    limitarSelecao();
}
// Tecla N: uma "partida" de um jogador sorteado entre poucos nomes, para ver
// o mesmo nome entrar uma vez, melhorar o recorde ou ser ignorado.
void simularFimDePartida() {
    const char* nomes[] = {"Ana", "ana ", "Caio", "Rafa", "Lu"};
    sementePartida = sementePartida * 1103515245u + 12345u;
    RegistroPontuacao r;
    r.data = "29/10/2026";
    r.jogador = nomes[(sementePartida >> 8) % 5];
    r.pontuacao = 100 * int((sementePartida >> 12) % 15);
    r.bombas = int((sementePartida >> 4) % 6);
    r.movimentos = 20 + int((sementePartida >> 16) % 40);
    auto antes = lerRanking();
    auto velho = std::find_if(antes.begin(), antes.end(), [&](const RegistroPontuacao& x) { return mesmoNome(x.jogador, r.jogador); });
    salvarRegistroPartida(r);
    std::wstring quem = strParaW(limparNomeRanking(r.jogador)) + L" fez " + std::to_wstring(r.pontuacao) + L" pts";
    if (velho == antes.end()) avisoRanking = quem + L": nome novo, entrou no ranking.";
    else if (melhorRegistro(r, *velho)) avisoRanking = quem + L": superou o recorde de " + std::to_wstring(velho->pontuacao) + L".";
    else avisoRanking = quem + L": nao superou " + std::to_wstring(velho->pontuacao) + L", recorde mantido.";
}

// ---------- teclado: o único lugar que muda o estado ----------
void lerTeclado(WPARAM tecla, LPARAM lParam) {
    if (lParam & (1 << 30)) return;            // repetição automática (Aula 30)
    if (confirmandoExclusao) {                 // a pergunta "bloqueia" a tela, como a MessageBoxW
        if (tecla == 'S' || tecla == VK_RETURN) responderExclusao(true);
        else if (tecla == 'N' || tecla == VK_ESCAPE) responderExclusao(false);
        return;
    }
    if (editandoNome) {
        if (tecla == VK_RETURN) terminarRenomeacaoRanking(true);
        else if (tecla == VK_ESCAPE) terminarRenomeacaoRanking(false);
        else if (tecla == VK_LEFT && cursorNome > 0) cursorNome--;
        else if (tecla == VK_RIGHT) cursorNome = std::min(cursorNome + 1, nomeEmEdicao.size());
        else if (tecla == VK_HOME) cursorNome = 0;
        else if (tecla == VK_END) cursorNome = nomeEmEdicao.size();
        else if (tecla == VK_DELETE && cursorNome < nomeEmEdicao.size()) nomeEmEdicao.erase(cursorNome, 1);
        return;                                // letras chegam por WM_CHAR
    }
    int total = totalRegistros();
    if (tecla == VK_UP) rankingSelecionado = std::max(0, rankingSelecionado - 1);
    else if (tecla == VK_DOWN) rankingSelecionado = std::min(std::max(0, total - 1), rankingSelecionado + 1);
    else if (tecla == VK_PRIOR) rankingSelecionado = std::max(0, rankingSelecionado - POR_PAGINA);
    else if (tecla == VK_NEXT) rankingSelecionado = std::min(std::max(0, total - 1), rankingSelecionado + POR_PAGINA);
    else if (tecla == VK_HOME) rankingSelecionado = 0;
    else if (tecla == VK_END) rankingSelecionado = std::max(0, total - 1);
    else if (tecla == 'E') iniciarRenomeacaoRanking();
    else if (tecla == VK_DELETE) pedirExclusaoRanking();
    else if (tecla == 'N') simularFimDePartida();
    else if (tecla == 'R') { limitarSelecao(); avisoRanking = L"Arquivo relido: " + std::to_wstring(totalRegistros()) + L" registros."; }
}
void digitarNomeJogador(wchar_t c) {
    if (!editandoNome) return;
    cursorNome = std::min(cursorNome, nomeEmEdicao.size());
    if (c == L'\b') { if (cursorNome > 0) nomeEmEdicao.erase(--cursorNome, 1); return; }
    if (c >= L' ' && c != 127 && nomeEmEdicao.size() < 24) { nomeEmEdicao.insert(cursorNome, 1, c); cursorNome++; avisoRanking.clear(); }
}

// ---------- escala e mouse (Aula 24 e Aula 30) ----------
float escala = 1.0f;
int margemX = 0, margemY = 0;
int px(int v) { return static_cast<int>(v * escala + 0.5f); }
void ajustarAreaDesenho(int largura, int altura) {
    if (largura <= 0 || altura <= 0) return;
    escala = std::min(float(largura) / LARGURA, float(altura) / ALTURA);
    margemX = (largura - px(LARGURA)) / 2;
    margemY = (altura - px(ALTURA)) / 2;
}
const int TOPO_LINHAS = 96, ALTURA_LINHA = 28;
void clicar(int xJanela, int yJanela) {
    int x = static_cast<int>((xJanela - margemX) / escala);
    int y = static_cast<int>((yJanela - margemY) / escala);
    if (confirmandoExclusao) {
        if (y >= 300 && y < 340) { if (x >= 200 && x < 360) responderExclusao(true); else if (x >= 400 && x < 560) responderExclusao(false); }
        return;
    }
    if (editandoNome) return;
    if (y >= TOPO_LINHAS && y < TOPO_LINHAS + POR_PAGINA * ALTURA_LINHA && x >= 20 && x < 740) {
        int alvo = rankingSelecionado / POR_PAGINA * POR_PAGINA + (y - TOPO_LINHAS) / ALTURA_LINHA;   // mesma conta do jogo
        if (alvo < totalRegistros()) rankingSelecionado = alvo;
    } else if (y >= 400 && y < 436) {
        if (x >= 20 && x < 220) iniciarRenomeacaoRanking();
        else if (x >= 236 && x < 436) pedirExclusaoRanking();
        else if (x >= 452 && x < 652) simularFimDePartida();
    }
}

// ---------- desenho: só lê o estado ----------
const Gdiplus::Color VERDE(255, 98, 235, 199), CIANO(255, 65, 210, 235), DOURADO(255, 252, 187, 65),
    CINZA(255, 144, 164, 190), CLARO(255, 220, 230, 240), VERMELHO(255, 255, 120, 100);
void painel(Gdiplus::Graphics& g, int x, int y, int l, int a, Gdiplus::Color cor = Gdiplus::Color(255, 25, 34, 50)) {
    Gdiplus::SolidBrush pincel(cor);
    g.FillRectangle(&pincel, px(x), px(y), px(l), px(a));
}
void desenharTexto(Gdiplus::Graphics& g, const std::wstring& texto, int x, int y, int tamanho, Gdiplus::Color cor = CLARO) {
    Gdiplus::Font fonte(L"Segoe UI", Gdiplus::REAL(px(tamanho)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(texto.c_str(), -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(x)), Gdiplus::REAL(px(y))), &pincel);
}
void botao(Gdiplus::Graphics& g, int x, const std::wstring& texto, Gdiplus::Color cor) {
    painel(g, x, 400, 200, 36);
    painel(g, x, 400, 4, 36, cor);
    desenharTexto(g, texto, x + 16, 407, 15, cor);
}

void desenharCena(Gdiplus::Graphics& g) {
    g.Clear(Gdiplus::Color(255, 11, 16, 28));
    painel(g, 0, 0, LARGURA, 5, CIANO);
    desenharTexto(g, L"RANKING", 20, 14, 28, CIANO);
    desenharTexto(g, L"Melhor resultado por nome. Empate: menos bombas, depois menos movimentos.", 20, 52, 12, CINZA);
    auto registros = lerRanking();
    if (registros.empty()) {
        desenharTexto(g, L"Nenhuma partida salva ainda. Aperte N para simular uma.", 20, 110, 16);
    } else {
        rankingSelecionado = std::clamp(rankingSelecionado, 0, int(registros.size()) - 1);
        int inicio = rankingSelecionado / POR_PAGINA * POR_PAGINA;
        desenharTexto(g, L"NOME", 56, 76, 11, CINZA);
        desenharTexto(g, L"PONTOS", 330, 76, 11, CINZA);
        desenharTexto(g, L"BOMBAS", 430, 76, 11, CINZA);
        desenharTexto(g, L"MOVIMENTOS", 520, 76, 11, CINZA);
        desenharTexto(g, L"DATA", 650, 76, 11, CINZA);
        for (int i = inicio; i < std::min(inicio + POR_PAGINA, int(registros.size())); ++i) {
            const auto& r = registros[i];
            int y = TOPO_LINHAS + (i - inicio) * ALTURA_LINHA;
            bool marcado = i == rankingSelecionado;
            if (marcado) painel(g, 20, y, 720, ALTURA_LINHA - 2, Gdiplus::Color(255, 35, 64, 81));
            Gdiplus::Color cor = i == 0 ? DOURADO : CLARO;
            desenharTexto(g, std::to_wstring(i + 1), 26, y + 4, 13, cor);
            std::wstring nome = strParaW(r.jogador);
            if (marcado && editandoNome) nome = L"(renomeando...)";
            desenharTexto(g, nome, 56, y + 3, 14, cor);
            desenharTexto(g, std::to_wstring(r.pontuacao), 330, y + 3, 14, cor);
            desenharTexto(g, std::to_wstring(r.bombas), 430, y + 4, 13, cor);
            desenharTexto(g, std::to_wstring(r.movimentos), 520, y + 4, 13, cor);
            desenharTexto(g, strParaW(r.data), 650, y + 4, 13, cor);
        }
        int paginas = (int(registros.size()) + POR_PAGINA - 1) / POR_PAGINA;
        desenharTexto(g, L"Pagina " + std::to_wstring(inicio / POR_PAGINA + 1) + L" de " + std::to_wstring(paginas) +
                         L"   (registro " + std::to_wstring(rankingSelecionado + 1) + L" de " + std::to_wstring(registros.size()) +
                         L")   SETAS selecionam, PgUp/PgDn mudam de pagina", 20, 378, 12, CINZA);
    }
    botao(g, 20, L"E   RENOMEAR", CLARO);
    botao(g, 236, L"DEL   EXCLUIR", VERMELHO);
    botao(g, 452, L"N   SIMULAR PARTIDA", VERDE);
    if (editandoNome) {
        painel(g, 20, 444, 720, 30, Gdiplus::Color(255, 30, 52, 66));
        painel(g, 20, 444, 4, 30, VERDE);
        // Mesmo formato para desenhar e medir, senão o cursor desloca (Aula 30).
        Gdiplus::Font fonte(L"Segoe UI", Gdiplus::REAL(px(15)), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::StringFormat formato(Gdiplus::StringFormat::GenericTypographic());
        formato.SetFormatFlags(formato.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
        Gdiplus::SolidBrush corNome(Gdiplus::Color(255, 246, 247, 251));
        std::wstring rotulo = L"NOVO NOME:  ";
        Gdiplus::RectF medidaRotulo, medida;
        g.MeasureString(rotulo.c_str(), -1, &fonte, Gdiplus::PointF(0, 0), &formato, &medidaRotulo);
        Gdiplus::SolidBrush corRotulo(CINZA);
        g.DrawString(rotulo.c_str(), -1, &fonte, Gdiplus::PointF(Gdiplus::REAL(px(34)), Gdiplus::REAL(px(450))), &formato, &corRotulo);
        float xNome = px(34) + medidaRotulo.Width;
        g.DrawString(nomeEmEdicao.c_str(), -1, &fonte, Gdiplus::PointF(xNome, Gdiplus::REAL(px(450))), &formato, &corNome);
        if ((relogio / 500) % 2 == 0) {
            std::wstring prefixo = nomeEmEdicao.substr(0, cursorNome);
            g.MeasureString(prefixo.c_str(), int(prefixo.size()), &fonte, Gdiplus::PointF(0, 0), &formato, &medida);
            Gdiplus::Pen caneta(VERDE, Gdiplus::REAL(std::max(1, px(2))));
            float cx = xNome + medida.Width + 1;
            g.DrawLine(&caneta, cx, Gdiplus::REAL(px(449)), cx, Gdiplus::REAL(px(469)));
        }
        desenharTexto(g, L"ENTER salva   ESC cancela", 560, 451, 12, CINZA);
    }
    if (confirmandoExclusao && !registros.empty()) {
        const auto& r = registros[rankingSelecionado];
        painel(g, 0, 0, LARGURA, ALTURA, Gdiplus::Color(170, 0, 0, 0));        // escurece a tela atrás
        painel(g, 160, 170, 440, 190, Gdiplus::Color(255, 30, 38, 54));
        painel(g, 160, 170, 440, 4, VERMELHO);
        desenharTexto(g, L"Excluir " + strParaW(r.jogador) + L" do ranking?", 184, 190, 20, VERMELHO);
        desenharTexto(g, L"A pontuacao de " + std::to_wstring(r.pontuacao) + L" pontos sera perdida.", 184, 228, 14);
        desenharTexto(g, L"O nome podera jogar de novo, comecando do zero.", 184, 252, 14, CINZA);
        painel(g, 200, 300, 160, 40, Gdiplus::Color(255, 70, 34, 34));
        desenharTexto(g, L"S   EXCLUIR", 222, 309, 16, VERMELHO);
        painel(g, 400, 300, 160, 40, Gdiplus::Color(255, 35, 48, 68));
        desenharTexto(g, L"N   CANCELAR", 420, 309, 16);
    }
    painel(g, 0, 482, LARGURA, 38, Gdiplus::Color(255, 18, 27, 43));
    desenharTexto(g, avisoRanking.empty() ? L"R rele o arquivo   HOME/END inicio e fim   ESC sai" : avisoRanking, 20, 492, 13, DOURADO);
    std::wstring arquivo = strParaW(caminhoRanking);
    size_t barra = arquivo.find_last_of(L"\\/");
    desenharTexto(g, arquivo.substr(barra == std::wstring::npos ? 0 : barra + 1), 590, 492, 12, CINZA);
}

// ---------- ranking de exemplo: duas páginas, um nome com ; e um com acento ----------
void criarRankingDeExemplo() {
    if (GetFileAttributesA(caminhoRanking.c_str()) != INVALID_FILE_ATTRIBUTES) return;
    std::vector<RegistroPontuacao> exemplo = {
        {"14/10/2026", "Davi", 900, 2, 50}, {"14/10/2026", "Bia", 900, 3, 35}, {"13/10/2026", "Ana", 900, 3, 40},
        {"13/10/2026", "Caio", 700, 2, 30}, {"12/10/2026", "João", 650, 4, 61}, {"12/10/2026", "Rafa", 600, 1, 44},
        {"11/10/2026", "Eva; a \"Rápida\"", 540, 2, 28}, {"11/10/2026", "Gil", 500, 5, 70}, {"10/10/2026", "Lia", 450, 2, 39},
        {"10/10/2026", "Theo", 400, 3, 52}, {"09/10/2026", "Nina", 350, 1, 25}, {"09/10/2026", "Otto", 300, 4, 66},
    };
    gravarRanking(exemplo);
    DeleteFileA((caminhoRanking + ".bak").c_str());   // o exemplo não conta como "versão anterior"
}

// ---------- janela ----------
HDC bufferDC = nullptr; HBITMAP bufferBitmap = nullptr; int bufferL = 0, bufferA = 0;
void desenhar(HWND janela) {
    PAINTSTRUCT ps;
    HDC destino = BeginPaint(janela, &ps);
    RECT r; GetClientRect(janela, &r);
    if (!bufferDC || bufferL != r.right || bufferA != r.bottom) {
        if (bufferDC) { DeleteDC(bufferDC); DeleteObject(bufferBitmap); }
        bufferDC = CreateCompatibleDC(destino);
        bufferBitmap = CreateCompatibleBitmap(destino, r.right, r.bottom);
        SelectObject(bufferDC, bufferBitmap);
        bufferL = r.right; bufferA = r.bottom;
    }
    {
        Gdiplus::Graphics g(bufferDC);
        Gdiplus::SolidBrush preto(Gdiplus::Color(255, 0, 0, 0));
        g.FillRectangle(&preto, 0, 0, r.right, r.bottom);
        g.TranslateTransform(Gdiplus::REAL(margemX), Gdiplus::REAL(margemY));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        desenharCena(g);
    }
    BitBlt(destino, 0, 0, r.right, r.bottom, bufferDC, 0, 0, SRCCOPY);
    EndPaint(janela, &ps);
}

LRESULT CALLBACK processarMensagem(HWND janela, UINT mensagem, WPARAM wParam, LPARAM lParam) {
    switch (mensagem) {
    case WM_CREATE:
        SetTimer(janela, 1, 100, nullptr);   // só para o cursor piscar
        return 0;
    case WM_TIMER:
        relogio = GetTickCount64();
        if (editandoNome) InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE && !editandoNome && !confirmandoExclusao) { DestroyWindow(janela); return 0; }
        lerTeclado(wParam, lParam);
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_CHAR:
        digitarNomeJogador(static_cast<wchar_t>(wParam));
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_LBUTTONUP:
        clicar(static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)));
        InvalidateRect(janela, nullptr, FALSE);
        return 0;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) { ajustarAreaDesenho(LOWORD(lParam), HIWORD(lParam)); InvalidateRect(janela, nullptr, FALSE); }
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        desenhar(janela);
        return 0;
    case WM_DESTROY:
        KillTimer(janela, 1);
        if (bufferDC) { DeleteDC(bufferDC); DeleteObject(bufferBitmap); }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(janela, mensagem, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdiPlus, &entrada, nullptr) != Gdiplus::Ok) return 1;
    criarRankingDeExemplo();
    WNDCLASSW classe{};
    classe.lpfnWndProc = processarMensagem;
    classe.hInstance = instancia;
    classe.lpszClassName = L"Aula32Ranking";
    classe.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClassW(&classe)) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 2; }
    RECT area{0, 0, LARGURA, ALTURA};
    AdjustWindowRect(&area, WS_OVERLAPPEDWINDOW, FALSE);
    HWND janela = CreateWindowExW(0, classe.lpszClassName, L"Aula 32 - tela de ranking", WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, area.right - area.left, area.bottom - area.top,
                                  nullptr, nullptr, instancia, nullptr);
    if (!janela) { Gdiplus::GdiplusShutdown(tokenGdiPlus); return 3; }
    ShowWindow(janela, exibir);
    MSG evento{};
    while (GetMessageW(&evento, nullptr, 0, 0) > 0) {
        TranslateMessage(&evento);
        DispatchMessageW(&evento);
    }
    Gdiplus::GdiplusShutdown(tokenGdiPlus);
    return 0;
}
