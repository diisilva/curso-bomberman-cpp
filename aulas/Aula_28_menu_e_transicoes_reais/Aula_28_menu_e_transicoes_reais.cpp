// Aula 28: exercita a rota real do menu do Bomberman original.
// Consulte bomberman.cpp: TelaMenu, desenharMenu, desenharSeletorFases, lerTeclado e desenhar.
#include "../../visual/visual_base.h" // Fornece janela Win32 e recursos GDI+.
#include <algorithm> // Limita os indices da fase e da dificuldade.
#include <array> // Guarda os nomes reais das dez fases.
#ifndef AULA28_FASE_INICIAL // O exercicio permite iniciar em outra arena.
#define AULA28_FASE_INICIAL 1 // O jogo comeca na primeira arena.
#endif // Fecha a configuracao da arena.
#ifndef AULA28_DIFICULDADE_INICIAL // O exercicio permite alterar o padrao.
#define AULA28_DIFICULDADE_INICIAL 1 // 0 facil, 1 medio, 2 dificil.
#endif // Fecha a configuracao da dificuldade.

namespace aula28 { // Mantem o estado do laboratorio separado do jogo final.
using namespace Gdiplus; // Encurta nomes de tipos graficos.
enum Tela {PRINCIPAL,JOGADORES,NOMES,CONTROLES,PERSONAGENS,DIFICULDADE,FASES,PARTIDA}; // Fluxo principal de telaMenu.
Tela tela=PRINCIPAL; // O jogo comeca em MENU_PRINCIPAL.
int fase=AULA28_FASE_INICIAL,dificuldade=AULA28_DIFICULDADE_INICIAL,jogadores=1,personagem=0; // Estado que a escolha muda.
bool automatico=false; // O bot da opcao 2 pula configuracao de jogadores.
std::unique_ptr<Bitmap> atlasCasino; // Tiles originais do preview de desenharMenu.
const wchar_t* nomes[]={L"CASINO AZUL",L"MASMORRA",L"FABRICA NEON",L"RUINAS DE PEDRA",
    L"CANION CEEDRUN",L"SELVA VIVA",L"CAVERNA CONGO",L"TEMPLO ARCANO",L"MUNDO BOMBER",L"VALE LANGRISSER"}; // NOMES_FASES.
const wchar_t* telas[]={L"PRINCIPAL",L"JOGADORES",L"NOMES",L"CONTROLES",L"PERSONAGENS",L"DIFICULDADE",L"FASES",L"PARTIDA"}; // Trilha visivel.

void carregar() { // PNG carregado somente depois de iniciar GDI+.
    visual::carregarAssets(); // Jogador e outros sprites do proprio jogo.
    if(!atlasCasino)atlasCasino=std::make_unique<Bitmap>((visual::pastaAssets()+L"SNES - Super Bomberman - Story Mode Stages - Stage 2-5.png").c_str()); // Fase 1.
}
void mosaico(Graphics& g,int x,int y,int cols,int rows,int medida) { // Monta o mesmo mini tabuleiro do menu original.
    if(!atlasCasino||atlasCasino->GetLastStatus()!=Ok)return; // Evita desenhar folha ausente.
    g.SetInterpolationMode(InterpolationModeNearestNeighbor); // Mantem pixel art nitida.
    for(int l=0;l<rows;++l)for(int c=0;c<cols;++c) { // Monta tiles por linha e coluna.
        bool parede=l==0||l==rows-1||c==0||c==cols-1||(l%2==0&&c%2==0); // Mesmo criterio do preview.
        bool caixa=!parede&&(l+c)%4==0; // Algumas caixas so para ilustrar o menu.
        Rect fonte=parede?Rect(16,16,16,16):caixa?Rect(48,32,16,16):Rect(96,48,16,16); // temaDaFase(1).
        g.DrawImage(atlasCasino.get(),Rect(x+c*medida,y+l*medida,medida,medida),fonte.X,fonte.Y,fonte.Width,fonte.Height,UnitPixel); // Desenha tile.
    }
}
void etapaDaTela() { // Mostra em qual capitulo do fluxo estamos.
    visual::passo=tela==PRINCIPAL?0:tela==FASES?2:tela==PARTIDA?3:1; // Quatro estados pedagogicos.
}
bool tecla(WPARAM k) { // Reproduz as transicoes principais de lerTeclado.
    if(k=='R'){tela=PRINCIPAL;automatico=false;etapaDaTela();return true;} // Reinicia a demonstracao.
    if(tela==PRINCIPAL) { // Primeira tela de desenharMenu.
        if(k=='1'){automatico=false;tela=JOGADORES;} // JOGAR entra na configuracao humana.
        else if(k=='2'){automatico=true;tela=DIFICULDADE;} // DEMONSTRACAO pula nomes e controles.
        else if(k=='8'){automatico=false;tela=FASES;} // Atalho real do seletor de fases.
        else return true; // Teclas sem exemplo ficam na tela atual.
    } else if(tela==JOGADORES) { // Escolhe um ou dois participantes.
        if(k>='1'&&k<='3'){jogadores=k=='1'?1:2;tela=NOMES;} // 2/3 correspondem a amigos/rivais no jogo.
        else if(k==VK_ESCAPE||k=='M')tela=PRINCIPAL; // Volta sem criar partida.
    } else if(tela==NOMES) { // No jogo real, o aluno digita os nomes.
        if(k==VK_RETURN)tela=CONTROLES; // Laboratorio usa os nomes P1/P2 para concentrar-se no fluxo.
        else if(k==VK_ESCAPE||k=='M')tela=JOGADORES; // Retorna a quantidade.
    } else if(tela==CONTROLES) { // Atribui teclado/joystick no jogo real.
        if(k==VK_RETURN)tela=PERSONAGENS; // Nesta aula, teclado ja pertence a P1.
        else if(k==VK_ESCAPE||k=='M')tela=NOMES; // Volta para corrigir nome.
    } else if(tela==PERSONAGENS) { // Escolha visual de jogador.
        if(k>='1'&&k<='4')personagem=int(k-'1'); // Quatro personagens do jogo.
        else if(k==VK_RETURN)tela=DIFICULDADE; // Segue para regras da campanha.
        else if(k==VK_ESCAPE||k=='M')tela=CONTROLES; // Retorna.
    } else if(tela==DIFICULDADE) { // Facil, medio ou dificil.
        if(k>='1'&&k<='3'){dificuldade=int(k-'1');tela=FASES;} // Teclas identicas ao original.
        else if(k==VK_RETURN)tela=FASES; // Confirma a escolha atual.
        else if(k==VK_ESCAPE||k=='M')tela=PRINCIPAL; // Sai da configuracao.
    } else if(tela==FASES) { // Seletor de dez arenas.
        if(k>='1'&&k<='9')fase=int(k-'0'); // Numero da arena.
        else if(k=='0')fase=10; // Zero significa arena 10.
        else if(k==VK_LEFT)fase=(fase+8)%10+1; // Mesma volta circular do original.
        else if(k==VK_RIGHT)fase=fase%10+1; // Avanca circularmente.
        else if(k==VK_UP||k==VK_DOWN)fase=(fase+4)%10+1; // Muda a linha dos cards.
        else if(k==VK_RETURN)tela=PARTIDA; // Equivale a novaPartida apos nomes validos.
        else if(k==VK_ESCAPE)tela=PRINCIPAL; // Sai do seletor.
        else if(k=='M')tela=DIFICULDADE; // Revê a dificuldade.
    } else if(tela==PARTIDA&&(k==VK_ESCAPE||k=='M'))tela=PRINCIPAL; // Retorna para rever escolhas.
    etapaDaTela(); // Sincroniza painel didatico.
    return true; // A tecla foi tratada antes do visualizador genérico.
}
void carta(Graphics& g,int x,int y,int largura,const std::wstring& rotulo,bool ativa=false) { // Cartao de menu.
    visual::caixa(g,x,y,largura,42,ativa?Color(255,54,86,86):Color(255,43,56,78),ativa?Color(255,98,235,199):Color(255,88,104,129)); // Pinta foco.
    visual::texto(g,rotulo,x+11,y+10,16,ativa?Color(255,156,245,205):Color(255,225,235,246)); // Desenha o comando.
}
void desenhar(Graphics& g,int,int desafio) { // Atualiza conteudo conforme Tela, sem redesenhar regra na entrada.
    carregar();etapaDaTela(); // Garante sprites e capitulo atuais.
    visual::titulo=L"Aula 28: menu e transicoes do jogo real"; // Identifica a aula.
    visual::rodape=L"Use as teclas dentro dos cartoes    Enter: confirmar    M/Esc: voltar    R: reiniciar"; // Orienta a pratica.
    visual::base(g,std::wstring(L"Estado atual: ")+telas[int(tela)]); // Cabecalho e rodape.
    visual::texto(g,L"Fonte: TelaMenu, lerTeclado, desenharMenu e desenharSeletorFases.",31,84,14,Color(255,142,213,206)); // Rastreabilidade.
    visual::caixa(g,27,116,557,398,Color(255,29,42,62)); // Painel da tela ativa.
    if(tela==PRINCIPAL) { // Replica opcoes de desenharMenu.
        visual::texto(g,L"BOMBERMAN",45,133,32,Color(255,246,247,251)); // Titulo do original.
        carta(g,45,185,305,L"1  JOGAR",true); // Inicia configuracao humana.
        carta(g,45,236,305,L"2  DEMONSTRACAO (IA)"); // Pula passos humanos.
        carta(g,45,287,305,L"8  SELETOR DE FASES"); // Atalho real.
        mosaico(g,365,175,7,7,28); // Mesmo atlas e desenho do preview.
        if(visual::spriteJogador&&visual::spriteJogador->GetLastStatus()==Ok)
            g.DrawImage(visual::spriteJogador.get(),Rect(410,250,43,63),16,0,16,24,UnitPixel); // Personagem real do menu.
        visual::texto(g,L"No original: 3-6 informacoes; 7 personagens; 9 Insano.",45,437,14); // Outros ramos.
    } else if(tela==FASES) { // Resume os dez cards de desenharSeletorFases.
        for(int i=0;i<10;++i){int x=43+(i%5)*105,y=153+(i/5)*122;
            visual::caixa(g,x,y,96,105,i+1==fase?Color(255,53,88,87):Color(255,42,55,76),i+1==fase?Color(255,98,235,199):Color(255,88,104,129)); // Realca escolhida.
            visual::texto(g,std::to_wstring(i+1),x+38,y+15,25); // Numero aceita 0 para dez.
            visual::texto(g,std::wstring(nomes[i]).substr(0,10),x+5,y+69,11); // Nome real.
        }
        visual::texto(g,L"1-9 / 0 / setas: escolher    Enter: iniciar",45,427,16,Color(255,252,187,65)); // Comandos reais.
        visual::texto(g,L"M: dificuldade    Esc: principal",45,462,15); // Caminhos de volta.
    } else if(tela==PARTIDA) { // Mostra a transferencia de escolhas ao jogo.
        visual::texto(g,L"novaPartida(modoPendente, dificuldade)",45,141,19,Color(255,252,187,65)); // Chamada real.
        mosaico(g,52,195,9,7,35); // Representa o primeiro quadro da arena.
        visual::texto(g,L"Fase "+std::to_wstring(fase)+L": "+nomes[fase-1],393,217,16); // Fase selecionada.
        visual::texto(g,automatico?L"Modo: IA":L"Modo: jogador",393,260,16); // Modo persistido.
        visual::texto(g,L"Dificuldade: "+std::to_wstring(dificuldade+1),393,300,15); // Parametro de montagem.
        visual::texto(g,L"montarFase() cria arena",393,353,13); // Ponte com Aula 27.
        visual::texto(g,L"desenharMapa() pinta",393,381,13); // Ponte com renderizacao.
    } else { // Telas intermediarias do percurso humano.
        const wchar_t* orientacao=tela==JOGADORES?L"1: solo    2: amigos    3: rivais":
            tela==NOMES?L"Nomes P1/P2: Enter confirma neste laboratorio":
            tela==CONTROLES?L"Teclado em P1: Enter confirma":
            tela==PERSONAGENS?L"1-4: sprite    Enter confirma":L"1: facil    2: medio    3: dificil"; // Acao por tela.
        visual::texto(g,telas[int(tela)],48,148,28,Color(255,252,187,65)); // Nome do estado.
        carta(g,48,226,505,orientacao,true); // Instrucao operacional.
        visual::texto(g,L"A escolha altera variaveis; o desenho apenas as mostra.",48,321,17); // Separacao regra/tela.
        visual::texto(g,L"O projeto original valida nomes e dispositivos",48,369,15); // Escopo real.
        visual::texto(g,L"antes de permitir uma partida manual.",48,398,15); // Guarda de transicao.
    }
    visual::caixa(g,604,116,326,398,Color(255,36,48,70)); // Fluxograma textual.
    visual::texto(g,L"ROTA DE JOGAR",623,139,19,Color(255,255,219,113)); // Titulo da trilha.
    for(int i=0;i<8;++i){ // Mostra todas as etapas, incluindo as ainda nao vistas.
        visual::texto(g,std::to_wstring(i+1)+L"  "+telas[i],623,182+i*38,16,i==int(tela)?Color(255,98,235,199):Color(255,203,213,226)); // Realce.
    }
    visual::caixa(g,27,526,903,62,Color(255,33,43,61)); // Area de pergunta/resposta.
    visual::texto(g,desafio?L"Desafio: preveja a proxima tela antes de apertar a tecla.":
        L"MENU escolhe opcoes; PARTIDA so comeca ao confirmar a fase.",42,546,17,Color(255,156,225,164)); // Conclusao.
}
bool salvar(Tela alvo,const wchar_t* nome) { // Captura uma tela para o roteiro.
    tela=alvo;Bitmap imagem(visual::LARGURA,visual::ALTURA);Graphics g(&imagem); // Reserva backbuffer.
    desenhar(g,0,0);return visual::salvarPng(imagem,nome); // Exporta PNG.
}
void liberar(){atlasCasino.reset();} // Destrói atlas antes de encerrar GDI+.
} // Fecha o namespace.
#ifndef CAPTURA // Janela interativa para o aluno.
int WINAPI WinMain(HINSTANCE instancia,HINSTANCE,LPSTR,int exibir){
    return visual::executar(instancia,exibir,L"Aula 28: menu e transicoes",aula28::desenhar,aula28::liberar,aula28::tecla); // Abre o loop.
}
#else // Modo de verificacao do build.
int main(){Gdiplus::GdiplusStartupInput entrada;ULONG_PTR token=0;
    if(Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok)return 1; // Inicializa GDI+.
    bool ok=aula28::salvar(aula28::PRINCIPAL,L"imagens/etapa_1.png")&&
        aula28::salvar(aula28::JOGADORES,L"imagens/etapa_2.png")&&
        aula28::salvar(aula28::FASES,L"imagens/etapa_3.png")&&
        aula28::salvar(aula28::PARTIDA,L"imagens/etapa_4.png"); // Quatro telas distintas.
    aula28::tela=aula28::PRINCIPAL;aula28::tecla('1');aula28::tecla('1');aula28::tecla(VK_RETURN);
    aula28::tecla(VK_RETURN);aula28::tecla(VK_RETURN);aula28::tecla('2');aula28::tecla('0');aula28::tecla(VK_RETURN); // Percorre rota.
    ok=ok&&aula28::tela==aula28::PARTIDA&&aula28::fase==10&&aula28::dificuldade==1; // Confere estado final.
    aula28::liberar();visual::liberarAssets();Gdiplus::GdiplusShutdown(token);return ok?0:2; // Fecha recursos.
}
#endif // Encerra os dois alvos de compilacao.
