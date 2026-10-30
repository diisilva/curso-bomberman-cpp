// Aula 27: reproduz a ordem de criarMapa, sortearPosicoesIniciais e posicionarBlocosBonus.
// Fonte: bomberman.cpp do projeto original, funcoes nas linhas 1093-1215 e temaDaFase.
#include "../../visual/visual_base.h" // Janela, GDI+, texto e exportacao PNG do curso.
#include <algorithm> // Embaralha as posicoes com a mesma ideia do jogo.
#include <array> // Guarda a matriz fixa de 13 por 15 celulas.
#include <random> // Sorteia paredes e posicoes com semente reproduzivel.
#include <vector> // Junta candidatas a bonus e posicoes iniciais.
#ifndef AULA27_CHANCE_CAIXA // Permite ao exercicio trocar a chance sem editar a regra.
#define AULA27_CHANCE_CAIXA 36 // Valor encontrado em criarMapa do jogo original.
#endif // Fecha a configuracao da chance.
#ifndef AULA27_SEMENTE // Permite repetir um mapa conhecido.
#define AULA27_SEMENTE 2026 // Semente fixa so para demonstracao reproduzivel.
#endif // Fecha a configuracao da semente.

namespace aula27 { // Isola os dados desta aula.
using namespace Gdiplus; // Encurta os tipos de desenho.
constexpr int linhas=13, colunas=15, lado=31; // Tamanho logico real e escala didatica.
enum Celula {VAZIO,SOLIDA,FRAGIL,BONUS}; // PORTAL so aparece depois da onda de inimigos.
using Mapa=std::array<std::array<Celula,colunas>,linhas>; // Acesso sempre como [linha][coluna].
struct Posicao {int l,c;}; // Guarda um par de coordenadas da grade.
struct Fase {Mapa mapa{};std::array<Posicao,8> spawns{};int caixas=0;int bonus=0;}; // Estado produzido pela montagem.
struct Tema {std::unique_ptr<Bitmap> chao,parede,caixa;}; // Tres recortes compactos por arena.
std::array<Tema,10> temas; // Nao reabre PNG a cada quadro.
std::unique_ptr<Bitmap> spriteInimigo; // Folha direcional real do adversario.
bool temasProntos=false,assetsOk=true; // Indica carregamento e diagnostico.
int faseAtual=1,semente=AULA27_SEMENTE; // F troca arena; N sorteia outro tabuleiro.
const wchar_t* nomes[]={L"CASINO AZUL",L"MASMORRA",L"FABRICA NEON",L"RUINAS DE PEDRA",
    L"CANION CEEDRUN",L"SELVA VIVA",L"CAVERNA CONGO",L"TEMPLO ARCANO",L"MUNDO BOMBER",L"VALE LANGRISSER"}; // Nomes reais.

std::unique_ptr<Bitmap> recorte(Image& folha,Rect origem) { // Copia so o tile utilizado.
    auto tile=std::make_unique<Bitmap>(46,46,PixelFormat32bppARGB); // Reserva 46 por 46 pixels.
    Graphics g(tile.get()); // Liga GDI+ ao bitmap compacto.
    g.SetInterpolationMode(InterpolationModeNearestNeighbor); // Preserva os pixels dos sprites.
    g.DrawImage(&folha,Rect(0,0,46,46),origem.X,origem.Y,origem.Width,origem.Height,UnitPixel); // Recorta e escala.
    return tile; // A folha grande pode ser liberada apos os tres recortes.
}
void carregar(int numero,const std::wstring& arquivo,Rect chao,Rect parede,Rect caixa) { // Liga fase ao atlas real.
    Bitmap folha((visual::pastaAssets()+arquivo).c_str()); // Resolve assets a partir da pasta de execucao.
    if(folha.GetLastStatus()!=Ok){assetsOk=false;return;} // Mostra erro na tela se o arquivo faltar.
    Tema& t=temas[numero-1]; // Usa indice de zero para a arena humana 1-10.
    t.chao=recorte(folha,chao);t.parede=recorte(folha,parede);t.caixa=recorte(folha,caixa); // Guarda somente tiles.
}
void carregarTemas() { // Replica as coordenadas de temaDaFase do jogo original.
    if(temasProntos)return; // Evita abrir dez folhas a cada WM_PAINT.
    temasProntos=true; // Marca a tentativa de carga.
    spriteInimigo=std::make_unique<Bitmap>((visual::pastaAssets()+L"inimigo_direcional.png").c_str()); // Carrega adversario real.
    carregar(1,L"SNES - Super Bomberman - Story Mode Stages - Stage 2-5.png",{96,48,16,16},{16,16,16,16},{48,32,16,16}); // Casino.
    carregar(2,L"GameCube - Bomberman Land 2 - Tilesets - Survival Bomberman Tiles (Dungeon).png",{315,760,32,32},{345,585,32,32},{696,696,32,32}); // Masmorra: copia uma vez.
    carregar(3,L"SNES - Super Bomberman - Story Mode Stages - Stage 6-3.png",{64,16,16,16},{0,0,16,16},{48,32,16,16}); // Fabrica.
    const int indices[]={0,0,0,2,3,5,0,7}; // INDICES_ATLAS_FASES para as arenas 4,5,6,8.
    for(int f : {4,5,6,8}) { // Le a folha compartilhada das arenas de estudo.
        int x=indices[f-1]*46; // Cada coluna do atlas mede 46 pixels.
        carregar(f,L"arenas_estudo.png",{x,0,46,46},{x,46,46,46},{x,92,46,46}); // Tres linhas: chao, parede, caixa.
    }
    carregar(7,L"arena_congo.png",{0,0,46,46},{0,46,46,46},{0,92,46,46}); // Arena Congo.
    carregar(9,L"arenas_novas.png",{0,0,46,46},{0,46,46,46},{0,92,46,46}); // Mundo Bomber.
    carregar(10,L"arenas_novas.png",{46,0,46,46},{46,46,46,46},{46,92,46,46}); // Vale Langrisser.
}
void limpar(Fase& f,Posicao p) { // Repete limparArea do jogo: centro e vizinhos ortogonais.
    for(Posicao q : {p,Posicao{p.l-1,p.c},Posicao{p.l+1,p.c},Posicao{p.l,p.c-1},Posicao{p.l,p.c+1}}) // Percorre cinco casas.
        if(q.l>0 && q.l<linhas-1 && q.c>0 && q.c<colunas-1)f.mapa[q.l][q.c]=VAZIO; // Nao remove a borda.
}
Fase montar(int etapa,int seed) { // Devolve o estado de uma etapa, sem alterar o jogo final.
    Fase f{};std::mt19937 gerador(seed); // F troca so a textura; N troca o mapa para comparar causas.
    std::uniform_int_distribution<int> chance(0,99); // Mesma faixa de criarMapa.
    for(int l=0;l<linhas;++l)for(int c=0;c<colunas;++c) { // Preenche 195 celulas.
        bool borda=l==0||l==linhas-1||c==0||c==colunas-1; // Limite intransponivel.
        bool pilar=l%2==0&&c%2==0; // Pilar fixo nas coordenadas pares.
        f.mapa[l][c]=borda||pilar?SOLIDA:etapa>=1&&chance(gerador)<AULA27_CHANCE_CAIXA?FRAGIL:VAZIO; // Valor padrao real: 36%.
    }
    if(etapa<2)return f; // Antes disso, ainda nao ha areas de inicio.
    f.spawns={Posicao{1,1},Posicao{1,colunas-2},Posicao{linhas-2,1},Posicao{linhas-2,colunas-2},
        Posicao{1,colunas/2},Posicao{linhas-2,colunas/2},Posicao{linhas/2,1},Posicao{linhas/2,colunas-2}}; // Oito candidatas reais.
    std::shuffle(f.spawns.begin(),f.spawns.end(),gerador); // Jogador e inimigos mudam de lugar.
    for(Posicao p:f.spawns)limpar(f,p); // Libera cinco casas em cada inicio.
    if(etapa>=3) { // Os bonus so sao escondidos depois de limpar as entradas.
        std::vector<Posicao> candidatas; // Guarda apenas caixas frageis.
        for(int l=1;l<linhas-1;++l)for(int c=1;c<colunas-1;++c)
            if(f.mapa[l][c]==FRAGIL)candidatas.push_back({l,c}); // Nao escolhe chao nem pilar.
        std::shuffle(candidatas.begin(),candidatas.end(),gerador); // Evita bonus sempre no mesmo ponto.
        f.bonus=std::min(8,int(candidatas.size())); // O jogo possui oito tipos de item.
        for(int i=0;i<f.bonus;++i)f.mapa[candidatas[i].l][candidatas[i].c]=BONUS; // Bloco continua parecendo caixa.
    }
    for(const auto& linha:f.mapa)for(Celula celula:linha)if(celula==FRAGIL||celula==BONUS)++f.caixas; // Conta obstaculos.
    return f; // O jogo acrescenta inimigos, bombas e portal em momentos posteriores.
}
bool tecla(WPARAM k) { // Controles do laboratorio, sem alterar o arquivo original.
    if(k=='F'){faseAtual=faseAtual%10+1;return true;} // Compara outro tileset da campanha.
    if(k=='N'){++semente;return true;} // Mostra outro sorteio reproduzivel.
    if(k=='R'){faseAtual=1;semente=AULA27_SEMENTE;return true;} // Recupera o caso inicial.
    return false; // Deixa as setas com o visualizador.
}
void desenhar(Graphics& g,int etapa,int desafio) { // Pinta mapa e explicacao lado a lado.
    visual::passo=etapa; // Indica a etapa correta no rodape das capturas.
    visual::carregarAssets(); // Prepara o personagem somente depois de iniciar GDI+.
    carregarTemas(); // Os PNGs reais sao preparados uma vez.
    visual::titulo=L"Aula 27: como nasce uma arena do Bomberman"; // Nomeia a aula.
    visual::rodape=L"Setas: 4 etapas   F: proxima arena   N: novo mapa   R: reiniciar   1/2: revelar/desafio"; // Legenda.
    const wchar_t* titulos[]={L"1. criarMapa: bordas e pilares",L"2. Sorteio de caixas frageis (36%)",
        L"3. Oito spawns e areas livres",L"4. Oito bonus ocultos e tema da fase"}; // Quatro perguntas progressivas.
    visual::base(g,titulos[etapa]); // Desenha titulo e rodape.
    Fase f=montar(etapa,semente); // Calcula o mesmo caso sem depender do desenho.
    Tema& tema=temas[faseAtual-1]; // Escolhe chao, parede e caixa da fase escolhida.
    int x0=27,y0=112; // Origem do tabuleiro desenhado.
    g.SetInterpolationMode(InterpolationModeNearestNeighbor); // Pixel art sem borrado.
    for(int l=0;l<linhas;++l)for(int c=0;c<colunas;++c) { // Desenha os 195 tiles.
        Celula t=f.mapa[l][c]; // Consulta a celula logica antes do sprite.
        Bitmap* tile=t==SOLIDA?tema.parede.get():t==FRAGIL||t==BONUS?tema.caixa.get():tema.chao.get(); // Mesmo bloco bonus nao se revela.
        Rect destino(x0+c*lado,y0+l*lado,lado,lado); // Converte coluna em X e linha em Y.
        if(tile)g.DrawImage(tile,destino); // Usa a arte real.
        else visual::caixa(g,destino.X,destino.Y,lado,lado,t==SOLIDA?Color(255,90,105,120):t==VAZIO?Color(255,39,105,78):Color(255,150,97,55)); // Diagnostico.
        if(etapa>=3&&t==BONUS){Pen marca(Color(255,255,210,60),2);g.DrawRectangle(&marca,destino);}
    }
    if(etapa>=2) { // Mostra o spawn do jogador e os de inimigos do modo facil.
        for(int i=0;i<4;++i){Posicao p=f.spawns[i];int x=x0+p.c*lado,y=y0+p.l*lado;
            if(i==0 && visual::spriteJogador && visual::spriteJogador->GetLastStatus()==Ok)
                g.DrawImage(visual::spriteJogador.get(),Rect(x+4,y-5,23,34),16,0,16,24,UnitPixel); // Sprite real do jogador.
            else if(i>0&&spriteInimigo&&spriteInimigo->GetLastStatus()==Ok)
                g.DrawImage(spriteInimigo.get(),Rect(x+4,y-5,23,34),16,0,16,24,UnitPixel); // Tres sprites reais de inimigo.
            else {SolidBrush b(i==0?Color(255,90,210,255):Color(255,240,90,90));g.FillEllipse(&b,x+6,y+6,19,19);} // Diagnostico de PNG.
        }
    }
    visual::caixa(g,510,112,421,402,Color(255,36,48,70)); // Painel de raciocinio.
    visual::texto(g,L"FASE "+std::to_wstring(faseAtual)+L" / 10  "+nomes[faseAtual-1],526,133,17,Color(255,255,219,113)); // Titulo da arena.
    visual::texto(g,L"13 linhas x 15 colunas = 195 casas",526,177,17); // Dimensao do original.
    visual::texto(g,L"pixelX = coluna * 52 no jogo",526,212,16); // Unidade real de desenho.
    visual::texto(g,L"pixelY = 104 + linha * 52",526,242,16); // Cabecalho antecede o mapa.
    if(etapa==0)visual::texto(g,L"borda OU (linha par E coluna par)",526,291,16); // Regra dos pilares.
    if(etapa==1)visual::texto(g,L"aleatorio(0..99) < "+std::to_wstring(AULA27_CHANCE_CAIXA)+L" -> caixa",526,291,16); // Probabilidade configuravel.
    if(etapa==2)visual::texto(g,L"embaralhar 8 pontos; limpar + vizinhos",526,291,15); // Reserva spawns.
    if(etapa==3)visual::texto(g,L"8 bonus em caixas, visual igual a fragil",526,291,15); // Segredo dos itens.
    visual::texto(g,L"Caixas: "+std::to_wstring(f.caixas)+L"   Bonus: "+std::to_wstring(f.bonus),526,339,17,Color(255,156,225,164)); // Resultado mensuravel.
    visual::texto(g,L"Semente: "+std::to_wstring(semente)+L"   F muda a textura",526,376,15); // Ajuda comparar.
    visual::texto(g,L"Portal surge so apos a onda de inimigos.",526,414,15); // Evita modelo incorreto.
    if(etapa==3)visual::texto(g,L"Borda amarela: raio X didatico dos bonus.",526,441,14,Color(255,255,219,113)); // No jogo nao ha contorno.
    if(!assetsOk)visual::texto(g,L"PNG ausente: confira projeto_final/assets",526,469,14,Color(255,255,125,125)); // Diagnostico.
    visual::caixa(g,27,526,904,62,Color(255,33,43,61)); // Caixa de verificacao.
    visual::texto(g,desafio?L"Desafio: preveja quais casas mudam na proxima etapa.":
        L"Compare o mesmo mapa em duas arenas: a matriz manda; o tileset apenas desenha.",42,544,17,Color(255,156,225,164)); // Resposta.
}
bool salvar(int etapa,int fase,const wchar_t* nome) { // Gera imagem para o roteiro.
    faseAtual=fase;Bitmap imagem(visual::LARGURA,visual::ALTURA);Graphics g(&imagem); // Cria quadro fora da tela.
    desenhar(g,etapa,0);return visual::salvarPng(imagem,nome); // Exporta PNG.
}
bool galeria() { // Coloca os dez conjuntos reais de tiles lado a lado.
    carregarTemas();Bitmap imagem(visual::LARGURA,visual::ALTURA);Graphics g(&imagem); // Prepara galeria fora da tela.
    g.Clear(Color(255,22,28,42));g.SetInterpolationMode(InterpolationModeNearestNeighbor); // Fundo e pixel art.
    visual::texto(g,L"DEZ ARENAS  |  um mapa logico, dez conjuntos de tiles",22,21,22,Color(255,255,225,100)); // Ideia da comparacao.
    visual::texto(g,L"Cada cartao: parede na borda, chao ao centro e caixa no meio.",22,59,15,Color(255,177,190,213)); // Legenda.
    for(int i=0;i<10;++i){int x=20+(i%5)*188,y=101+(i/5)*250;Tema& t=temas[i]; // Dois grupos de cinco fases.
        visual::caixa(g,x,y,174,222,Color(255,35,47,69)); // Separa visualmente as arenas.
        visual::texto(g,std::to_wstring(i+1)+L". "+nomes[i],x+8,y+9,12,Color(255,255,219,113)); // Nome real.
        for(int l=0;l<3;++l)for(int c=0;c<4;++c){ // Mini tabuleiro comum para comparar somente o tema.
            Bitmap* tile=l==0||l==2||c==0||c==3?t.parede.get():l==1&&c==2?t.caixa.get():t.chao.get(); // Mesma classe logica.
            if(tile)g.DrawImage(tile,Rect(x+15+c*36,y+40+l*36,36,36)); // Amostra do PNG real.
        }
        visual::texto(g,L"parede / chao / caixa",x+9,y+162,11,Color(255,156,225,164)); // Ordem dos tipos.
        visual::texto(g,i==1?L"Dungeon 32 px":i==0||i==2?L"SNES 16 px":L"Atlas 46 px",x+9,y+188,12); // Origem do recorte.
    }
    visual::texto(g,L"Fases 4, 5, 6 e 8 usam arenas_estudo.png; 7 usa arena_congo.png; 9-10 usam arenas_novas.png.",22,604,13); // Atlas compartilhados.
    return assetsOk&&visual::salvarPng(imagem,L"imagens/galeria_10_fases.png"); // Falha se qualquer folha faltar.
}
void liberar(){spriteInimigo.reset();for(Tema& t:temas){t.chao.reset();t.parede.reset();t.caixa.reset();}} // Fecha recortes antes de GDI+.
} // Encerra a aula.
#ifndef CAPTURA // O programa comum abre janela interativa.
int WINAPI WinMain(HINSTANCE instancia,HINSTANCE,LPSTR,int exibir){
    return visual::executar(instancia,exibir,L"Aula 27: cenarios e tabuleiro",aula27::desenhar,aula27::liberar,aula27::tecla); // Entra no loop Win32.
}
#else // O build de verificacao gera os PNGs e testa invariantes.
int main(){Gdiplus::GdiplusStartupInput entrada;ULONG_PTR token=0;
    if(Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok)return 1; // Inicia GDI+.
    auto f=aula27::montar(3,2026); // Testa o caso real antes de salvar.
    bool ok=f.mapa[0][0]==aula27::SOLIDA&&f.mapa[2][2]==aula27::SOLIDA&&f.bonus==8;
    ok=aula27::salvar(0,1,L"imagens/etapa_1.png")&&aula27::salvar(1,2,L"imagens/etapa_2.png")&&
       aula27::salvar(2,4,L"imagens/etapa_3.png")&&aula27::salvar(3,10,L"imagens/etapa_4.png")&&
       aula27::galeria()&&ok; // Quatro estados e um comparativo das dez arenas.
    aula27::liberar();visual::liberarAssets();Gdiplus::GdiplusShutdown(token);return ok?0:2; // Libera recursos.
}
#endif // Encerra a escolha entre GUI e captura.
