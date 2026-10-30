// Aula 26. Galeria interativa dos cinco chefes reais da campanha.
// Recortes e regras: projeto_final/bomberman.cpp, carregarBossesCampanha,
// carregarAssets, criarPoderBoss, atualizarBoss e desenharBossAnimado.
#include "../../visual/visual_base.h"
#include <array>
#include <cstdio>
#include <cmath>

#ifndef CHEFE_INICIAL_AULA
#define CHEFE_INICIAL_AULA 0
#endif

namespace aula26 {
using namespace Gdiplus;
struct Chefe {
    const wchar_t* nome;
    const wchar_t* arquivo;
    int fase,vida,x,y,largura,altura;
    const wchar_t* poder1; const wchar_t* poder2; const wchar_t* poder3;
    const wchar_t* caso1; const wchar_t* caso2;
};
constexpr std::array<Chefe,5> chefes{{
    {L"GOLEM DA MASMORRA",L"SNES - Final Fantasy V - Enemies & Bosses - Enemies.png",2,55,656,16,40,64,
     L"Machado: 2,8 celulas/s",L"Projeteis miram o jogador",L"Ataque a cada ~3,91 s",
     L"Distancia 3 / 2,8 = 1,07 s",L"A parede interrompe o machado."},
    {L"EGG-HVC / DEMOLIDOR",L"Sega CD - Sonic the Hedgehog CD - Bosses - Palmtree Panic Boss - EGG-HVC-001.png",4,74,869,424,112,124,
     L"Energia: 3,6 celulas/s",L"Investida com aviso de 1 s",L"Durante a investida: 4,8 cel/s",
     L"Aviso: linha tracejada antes",L"Energia e avancada podem coincidir."},
    {L"HORNET DA SELVA",L"hornet_animacoes.png",6,92,0,0,160,160,
     L"Agulha, dash e salto",L"Giro solta fios; guarda reage",L"5 acoes em sequencia",
     L"Preparo da agulha: 750 ms",L"Guarda dura 1200 ms; pode contra-atacar."},
    {L"GUARDIAO ARCANO",L"boss.png",8,115,0,0,253,253,
     L"Chuva de fogo direcionada",L"Fogo viaja a 3,2 celulas/s",L"Fogo destrói caixa fragil",
     L"Mira antecipa o alvo",L"Fogo deixa de existir ao atingir parede."},
    {L"BOWSER",L"bowser_animacoes.png",10,129,0,0,160,160,
     L"Rajada de 3 fogos",L"Investida e salto anunciado",L"Anel e disparo triplo",
     L"Ao pousar: 8 projeteis em anel",L"No ataque 4: tres tiros a 2,8 cel/s."}
}};
int selecionado=CHEFE_INICIAL_AULA;
std::array<std::unique_ptr<Bitmap>,5> retratos;
std::unique_ptr<Bitmap> recortar(Bitmap& folha,const Chefe& c,bool chave) {
    auto recorte=std::make_unique<Bitmap>(c.largura,c.altura,PixelFormat32bppARGB);
    { Graphics g(recorte.get()); g.Clear(Color(0,0,0,0));
      g.DrawImage(&folha,Rect(0,0,c.largura,c.altura),c.x,c.y,c.largura,c.altura,UnitPixel); }
    if(chave){
        Color fundo;recorte->GetPixel(0,0,&fundo);
        for(int y=0;y<c.altura;++y)for(int x=0;x<c.largura;++x){Color p;recorte->GetPixel(x,y,&p);
            if(p.GetValue()==fundo.GetValue() || (p.GetR()==0&&p.GetG()==44&&p.GetB()==76) ||
               (p.GetR()==16&&p.GetG()==112&&p.GetB()==132) || (p.GetR()==65&&p.GetG()==169&&p.GetB()==184))
                recorte->SetPixel(x,y,Color(0,0,0,0));
            if(c.fase==4 && p.GetR()==153&&p.GetG()==217&&p.GetB()==234)
                recorte->SetPixel(x,y,Color(0,0,0,0)); // Faixa azul do atlas Egg.
        }
    }
    return recorte;
}
Bitmap* imagem(int indice){
    if(retratos[indice])return retratos[indice].get();
    const Chefe& c=chefes[indice];Bitmap folha((visual::pastaAssets()+c.arquivo).c_str());
    if(folha.GetLastStatus()!=Ok)return nullptr;
    retratos[indice]=recortar(folha,c,indice<2);
    return retratos[indice].get();
}
void liberar(){for(auto& r:retratos)r.reset();visual::liberarAssets();}
bool tecla(WPARAM t){if(t>='1'&&t<='5'){selecionado=int(t-'1');return true;}return false;}
void textoPoder(Graphics& g,const wchar_t* frase,int y){visual::texto(g,L"• "+std::wstring(frase),489,y,17);}
void desenhar(Graphics& g,int passo,int){
    visual::passo=passo;
    visual::rodape=L"1-5: escolher chefe    Setas: regra 1-4    Espaco: animar/pausar";
    visual::titulo=L"Aula 26: cinco chefes reais do Bomberman";
    const wchar_t* etapas[]={L"1. Arquivo de imagem e recorte usados no jogo final.",
        L"2. Fase, vida e ciclo de atualizacao do chefe.",
        L"3. Poderes, projeteis e tempos do chefe escolhido.",
        L"4. Caso calculado: preveja e confira o resultado."};
    visual::base(g,etapas[passo]);
    const Chefe& c=chefes[selecionado];
    visual::texto(g,L"Fases pares: 2, 4, 6, 8, 10. Indice = fase / 2 - 1.",38,88,15,Color(255,142,213,206));
    for(int i=0;i<5;++i){
        int x=36+i*181;
        visual::caixa(g,x,116,170,43,i==selecionado?Color(255,106,83,41):Color(255,39,52,73),
                      i==selecionado?Color(255,255,220,105):Color(255,90,105,135));
        visual::texto(g,std::to_wstring(i+1)+L"  FASE "+std::to_wstring(chefes[i].fase),x+10,126,17);
    }
    visual::caixa(g,35,171,404,303,Color(255,38,48,67));
    auto* sprite=imagem(selecionado);
    if(sprite){
        g.SetInterpolationMode(InterpolationModeNearestNeighbor);
        int w=std::min(320,int(280.f*c.largura/std::max(c.largura,c.altura)));
        int h=std::min(280,int(280.f*c.altura/std::max(c.largura,c.altura)));
        g.DrawImage(sprite,Rect(237-w/2,190+(266-h)/2,w,h),0,0,c.largura,c.altura,UnitPixel);
    } else visual::texto(g,L"Sprite ausente: confira assets.",72,283,17,Color(255,255,130,130));
    visual::caixa(g,44,435,385,32,Color(235,29,36,50));
    visual::texto(g,c.nome,54,441,17,Color(255,255,220,105));
    visual::caixa(g,464,171,458,303,Color(255,39,52,73));
    if(passo==0){
        visual::texto(g,L"Carregamento e recorte",486,189,21);
        visual::texto(g,L"carregarSprite -> recortarEmBitmapCompacto",486,231,15);
        visual::texto(g,L"Origem: ("+std::to_wstring(c.x)+L","+std::to_wstring(c.y)+L")",486,267,17);
        visual::texto(g,L"Quadro: "+std::to_wstring(c.largura)+L" x "+std::to_wstring(c.altura)+L" px",486,298,17);
        visual::texto(g,selecionado<2?L"Fundo removido pela cor do canto.":L"Folha PNG com transparencia.",486,340,16);
        visual::texto(g,L"Nome completo do PNG: veja o roteiro.",486,398,15);
    } else if(passo==1){
        visual::texto(g,L"Estado da luta",486,189,21);
        visual::texto(g,L"faseAtual = "+std::to_wstring(c.fase),486,231,18);
        visual::texto(g,L"vidaMaximaBoss = "+std::to_wstring(c.vida),486,267,18);
        visual::texto(g,L"base * FATOR_FORCA_BOSS (1,15)",486,303,15);
        visual::texto(g,L"corpo: "+std::to_wstring(selecionado==2?2:3)+L" x 2 celulas",486,339,18);
        visual::texto(g,L"atualizarBoss(dt) move e ataca.",486,397,16);
    } else if(passo==2){
        visual::texto(g,L"Poderes reais",486,189,21);
        textoPoder(g,c.poder1,231);textoPoder(g,c.poder2,275);textoPoder(g,c.poder3,319);
        visual::texto(g,L"criarPoderBoss -> atualizarBoss",486,399,16,Color(255,142,213,206));
    } else {
        visual::texto(g,L"Caso real e previsao",486,189,21);
        visual::texto(g,c.caso1,486,243,17,Color(255,255,220,105));
        visual::texto(g,c.caso2,486,281,16);
        visual::texto(g,L"Reproduza no jogo final e compare",486,360,16);
        visual::texto(g,L"o estado antes e depois do ataque.",486,390,16);
    }
    visual::caixa(g,36,496,888,92,Color(255,33,43,61));
    visual::texto(g,L"Imagem e parametros extraidos do projeto final; cada tecla 1-5 troca o chefe.",52,514,17,Color(255,156,225,164));
    visual::texto(g,L"Setas mostram recorte, vida, poderes e caso. Fonte: projeto_final/bomberman.cpp.",52,549,15);
}
bool salvar(int indice,int etapa,const wchar_t* nome){
    selecionado=indice;Bitmap imagem(visual::LARGURA,visual::ALTURA);
    Graphics g(&imagem);g.SetSmoothingMode(SmoothingModeAntiAlias);
    desenhar(g,etapa,0);return visual::salvarPng(imagem,nome);
}
}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia,HINSTANCE,LPSTR,int exibir){
    return visual::executar(instancia,exibir,L"Aula 26: cinco chefes reais",aula26::desenhar,aula26::liberar,aula26::tecla);
}
#else
int main(){
    Gdiplus::GdiplusStartupInput entrada;ULONG_PTR token=0;
    if(Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok)return 1;
    bool ok=aula26::salvar(0,0,L"imagens\\etapa_1.png") && aula26::salvar(4,3,L"imagens\\etapa_4.png");
    for(int i=0;i<5;++i){wchar_t nome[80];swprintf(nome,80,L"imagens\\chefe_%d.png",i+1);
        ok=aula26::salvar(i,2,nome)&&ok;}
    aula26::liberar();Gdiplus::GdiplusShutdown(token);
    return ok && aula26::chefes[4].vida==129 && aula26::chefes[2].fase==6?0:2;
}
#endif
