// Aula 25. Mario aliado: movimento, escolha do alvo, voo e colisao da maca.
// Dados reproduzidos de projeto_final/bomberman.cpp: MarioAliado,
// lancarMacaMario, atualizarMario e desenharMacasMario.
#include "../../visual/visual_base.h"
#include <cmath>
#include <cstdio>

#ifndef FATOR_MARIO_AULA
#define FATOR_MARIO_AULA 0.765f
#endif
namespace aula25 {
using namespace Gdiplus;
std::unique_ptr<Bitmap> folhaMario,folhaChefe;
constexpr float velocidade=4.2f*FATOR_MARIO_AULA;
constexpr float pi=3.1415926f;
int amostraMovimento=0;
float colunaVisual(){return 2.f+std::min(1.f,velocidade*.1f*amostraMovimento);}
bool tecla(WPARAM t){
    if(visual::passo!=0)return false;
    if(t=='D'){amostraMovimento=std::min(4,amostraMovimento+1);return true;}
    if(t=='A'){amostraMovimento=std::max(0,amostraMovimento-1);return true;}
    return false;
}
std::wstring numero(float valor){wchar_t buf[32];swprintf(buf,32,L"%.3f",valor);return buf;}
void iniciar() {
    visual::carregarAssets();
    if (!folhaMario) folhaMario=std::make_unique<Bitmap>((visual::pastaAssets()+L"mario.png").c_str());
    if (!folhaChefe) folhaChefe=std::make_unique<Bitmap>((visual::pastaAssets()+L"boss.png").c_str());
}
void liberar() { folhaMario.reset(); folhaChefe.reset(); visual::liberarAssets(); }
void mario(Graphics& g,int x,int y,int quadro=1,int direcao=0,int tamanho=66) {
    if (!folhaMario || folhaMario->GetLastStatus()!=Ok) return;
    int origemX=(direcao==0?60:direcao==1?114:6)+quadro*17;
    int origemY=direcao==2?24:6;
    ImageAttributes transparencia;
    transparencia.SetColorKey(Color(255,56,135,0),Color(255,56,135,0));
    Rect destino(x,y,tamanho,tamanho);
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g.DrawImage(folhaMario.get(),destino,origemX,origemY,16,16,UnitPixel,&transparencia);
}
void maca(Graphics& g,int x,int y,int escala=2) {
    SolidBrush vermelho(Color(255,241,61,57)),folha(Color(255,107,220,75)),talo(Color(255,114,73,42));
    g.FillEllipse(&vermelho,x,y+4*escala,10*escala,9*escala);
    g.FillRectangle(&talo,x+5*escala,y,2*escala,5*escala);
    g.FillEllipse(&folha,x+7*escala,y+escala,4*escala,2*escala);
}
void terreno(Graphics& g,int x,int y,int colunas,int linhas,int lado) {
    auto* atlas=visual::spriteTerreno.get();
    if (!atlas || atlas->GetLastStatus()!=Ok) return;
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for(int l=0;l<linhas;++l) for(int c=0;c<colunas;++c){
        int tipo=(l==0||c==0||l==linhas-1||c==colunas-1)?46:0;
        g.DrawImage(atlas,Rect(x+c*lado,y+l*lado,lado,lado),7*46,tipo,46,46,UnitPixel);
    }
}
void chefe(Graphics& g,int x,int y) {
    visual::caixa(g,x,y,3*53,2*53,Color(255,98,49,74),Color(255,238,135,91));
    if(folhaChefe && folhaChefe->GetLastStatus()==Ok)
        g.DrawImage(folhaChefe.get(),Rect(x+48,y+4,64,80),0,0,253,253,UnitPixel);
    visual::texto(g,L"CORPO: 3 x 2 CELULAS",x+9,y+86,12);
}
void desenhar(Graphics& g,int passo,int variante) {
    iniciar();
    visual::passo=passo; visual::variante=variante;
    visual::rodape=L"Setas: etapa 1-4    A/D: mover Mario na etapa 1    1/2: resposta/desafio";
    visual::titulo=L"Aula 25: Mario aliado e a maca";
    const wchar_t* subtitulos[]={L"1. Reserva uma celula; o desenho se aproxima com dt.",
        L"2. Ao atacar, escolhe o alvo mais proximo naquele instante.",
        L"3. A maca voa ate um destino fixo; a curva e apenas visual.",
        L"4. Ao chegar, testa celula e aplica 3 de dano ao chefe."};
    visual::base(g,subtitulos[passo]);
    visual::texto(g,L"Caso real: MarioAliado, lancarMacaMario e atualizarMario no jogo final.",38,88,15,Color(255,142,213,206));
    terreno(g,40,132,9,6,53);
    const int mx=40+int((passo==0?colunaVisual():4.f)*53),my=132+3*53;
    if(passo==0){Pen destino(Color(255,255,220,105),3);g.DrawRectangle(&destino,40+3*53,132+3*53,53,53);}
    mario(g,mx,my-15,passo==0?0:1,3,70);
    if(passo>=1) {
        chefe(g,40+5*53,132+2*53);
        visual::linha(g,mx+35,my+20,40+5*53+79,132+2*53+53,Color(255,248,210,97),2);
    }
    if(passo>=2){
        float u=passo==2?.50f:1.f;
        int sx=mx+35,sy=my+25,tx=40+5*53+79,ty=132+2*53+53;
        int x=int(sx+(tx-sx)*u),chao=int(sy+(ty-sy)*u);
        int altura=int(std::sin(u*pi)*53*1.7f+(1-u)*20);
        maca(g,x-11,chao-altura-15,2);
        if(passo==3)visual::texto(g,L"-3 HP",tx-14,ty-58,20,Color(255,255,135,110));
    }
    visual::caixa(g,555,132,366,334,Color(255,39,52,73));
    if(passo==0){
        visual::texto(g,L"Movimento logico e visual",573,155,20);
        visual::texto(g,L"destino: proxima celula livre",573,207,17);
        visual::texto(g,L"passo = min(distancia, v * dt)",573,243,16);
        visual::texto(g,L"v = 4,2 * "+numero(FATOR_MARIO_AULA)+L" = "+numero(velocidade)+L" cel/s",573,281,16);
        visual::texto(g,L"1 celula leva ~"+std::to_wstring(int(1000/velocidade))+L" ms.",573,319,16);
        visual::texto(g,L"logicaC=3; visualC="+numero(colunaVisual()),573,355,16,Color(255,156,225,164));
        visual::texto(g,L"A/D: avancar ou voltar 0,1 s.",573,390,15);
    } else if(passo==1){
        visual::texto(g,L"Escolha de alvo",573,155,20);
        visual::texto(g,L"1. inimigo mais proximo",573,205,17);
        visual::texto(g,L"2. jogador rival, se houver",573,241,17);
        visual::texto(g,L"3. chefe, se for o mais perto",573,277,17);
        visual::texto(g,L"Alvo = centro da celula/corpo.",573,330,16);
        visual::texto(g,L"Sem alvo: 3 casas a frente.",573,365,16);
    } else if(passo==2){
        visual::texto(g,L"Tempo e trajetoria",573,155,20);
        visual::texto(g,L"origem = centro do Mario",573,207,17);
        visual::texto(g,L"distancia = hipot(dx, dy)",573,242,17);
        visual::texto(g,L"duracao = clamp(d/5, .65, 1.45)",573,278,16);
        visual::texto(g,L"u = idade / duracao",573,315,17);
        visual::texto(g,L"altura = sin(u*pi)*1,7 cel",573,351,16);
        visual::texto(g,L"Recarga entre macas: 1,05 s.",573,394,16);
    } else {
        visual::texto(g,L"Assertividade do acerto",573,155,20);
        visual::texto(g,L"No pouso: p=(int(ty), int(tx))",573,207,16);
        visual::texto(g,L"celulaBoss(p) ? dano=3 : errou",573,245,16);
        visual::texto(g,L"Chefe em movimento pode sair",573,295,16);
        visual::texto(g,L"da celula ate a chegada.",573,322,16);
        visual::texto(g,L"Imunidade do chefe: 700 ms.",573,372,16);
        visual::texto(g,L"Maca tambem quebra caixa.",573,404,16);
    }
    visual::caixa(g,36,496,888,92,Color(255,33,43,61));
    if(variante==1) visual::texto(g,L"Desafio: preveja a proxima posicao, o tempo ou o dano.",52,522,19,Color(255,255,220,105));
    else {
        const std::wstring respostas[]={L"4,2 * "+numero(FATOR_MARIO_AULA)+L" = "+numero(velocidade)+L" celulas por segundo.",L"O chefe so vira alvo se estiver mais perto que os demais.",
            L"A velocidade horizontal e distancia / duracao; a curva nao muda o alvo.",L"Acerta apenas se a celula de pouso ainda pertence ao corpo do chefe."};
        visual::texto(g,respostas[passo],52,522,17,Color(255,156,225,164));
    }
    visual::texto(g,L"Na etapa 1, A/D muda o tempo; a borda amarela marca a celula logica reservada.",52,555,14);
}
bool salvar(int p,const wchar_t* nome) {
    Bitmap imagem(visual::LARGURA,visual::ALTURA);
    Graphics g(&imagem);g.SetSmoothingMode(SmoothingModeAntiAlias);
    desenhar(g,p,0);return visual::salvarPng(imagem,nome);
}
}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia,HINSTANCE,LPSTR,int exibir){
    return visual::executar(instancia,exibir,L"Aula 25: Mario aliado e a maca",aula25::desenhar,aula25::liberar,aula25::tecla);
}
#else
int main(){
    Gdiplus::GdiplusStartupInput entrada;ULONG_PTR token=0;
    if(Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok)return 1;
    bool ok=aula25::salvar(0,L"imagens\\etapa_1.png") && aula25::salvar(3,L"imagens\\etapa_4.png");
    aula25::liberar();Gdiplus::GdiplusShutdown(token);
    aula25::amostraMovimento=0;visual::passo=0;
    if(!aula25::tecla('D') || !aula25::tecla('D') ||
       std::fabs(aula25::colunaVisual()-(2.f+std::min(1.f,aula25::velocidade*.2f)))>.001f)return 3;
    return ok?0:2;
}
#endif
