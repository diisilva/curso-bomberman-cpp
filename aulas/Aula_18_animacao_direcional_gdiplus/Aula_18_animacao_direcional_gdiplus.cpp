// Aula 18: o recorte irregular da Aula 05 agora anda pelo mapa.
// Setas escolhem uma celula livre. O desenho interpola e troca pose por distancia.
#include <windows.h>
#include <gdiplus.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

enum Direcao { CIMA, ESQUERDA, BAIXO, DIREITA };
const wchar_t* nomes[4]={L"CIMA",L"ESQUERDA",L"BAIXO",L"DIREITA"};
const char* mapa[5]={"#######","#.....#","#..#..#","#.....#","#######"};
const int quadroX[4][3]={{18,36,54},{73,92,111},{131,149,167},{186,205,224}};
const int quadroLargura[4]={17,18,17,18};
const int ciclo[4]={0,1,0,2};
ULONG_PTR tokenGdi=0;
std::unique_ptr<Gdiplus::Bitmap> folha;
Direcao direcao=BAIXO;
int logicaL=1,logicaC=1,indiceCiclo=0;
float visualL=1,visualC=1,distancia=0;
constexpr int LARGURA=920,ALTURA=680;

std::wstring caminhoDaFolha() {
    const wchar_t* pastas[]={L"projeto_final\\assets\\",L"..\\projeto_final\\assets\\",L"..\\..\\projeto_final\\assets\\"};
    for (const auto* pasta:pastas)
        if (GetFileAttributesW(pasta)!=INVALID_FILE_ATTRIBUTES)
            return std::wstring(pasta)+L"personagem_principal_sprite_v2.png";
    return std::wstring(pastas[0])+L"personagem_principal_sprite_v2.png";
}
bool carregarFolha() {
    folha=std::make_unique<Gdiplus::Bitmap>(caminhoDaFolha().c_str());
    return folha->GetLastStatus()==Gdiplus::Ok;
}
Gdiplus::Rect origemAtual() {
    int passo=ciclo[indiceCiclo];
    return Gdiplus::Rect(quadroX[direcao][passo],38,quadroLargura[direcao],26);
}
void atualizarMovimento() {
    auto aproximar=[](float& visual,int logica) {
        float antes=visual;
        visual+=std::clamp(float(logica)-visual,-0.08f,0.08f);
        return std::abs(visual-antes);
    };
    distancia+=aproximar(visualL,logicaL)+aproximar(visualC,logicaC);
    if (distancia>=0.25f) { distancia-=0.25f; indiceCiclo=(indiceCiclo+1)%4; }
    if (visualL==logicaL && visualC==logicaC) { indiceCiclo=0; distancia=0; }
}
void texto(Gdiplus::Graphics& g,const std::wstring& s,int x,int y,int tamanho,Gdiplus::Color cor) {
    Gdiplus::FontFamily familia(L"Consolas");
    Gdiplus::Font fonte(&familia,Gdiplus::REAL(tamanho),Gdiplus::FontStyleRegular,Gdiplus::UnitPixel);
    Gdiplus::SolidBrush pincel(cor);
    g.DrawString(s.c_str(),-1,&fonte,Gdiplus::PointF(x,y),&pincel);
}
void desenharCena(Gdiplus::Graphics& g) {
    const Gdiplus::Color claro(255,235,240,250),ouro(255,255,220,100);
    g.Clear(Gdiplus::Color(255,24,31,46));
    texto(g,L"Aula 18: direcao + distancia = animacao",25,18,25,ouro);
    texto(g,L"Setas movem. A origem troca de pose; o destino desliza entre celulas.",25,56,16,claro);
    if (!folha || folha->GetLastStatus()!=Gdiplus::Ok) {
        texto(g,L"PNG ausente: execute da raiz e confira projeto_final/assets.",25,115,18,Gdiplus::Color(255,255,100,100));
        return;
    }
    g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    Gdiplus::Rect origem=origemAtual();
    g.DrawImage(folha.get(),Gdiplus::Rect(25,115,464,80),14,30,232,40,Gdiplus::UnitPixel);
    Gdiplus::Pen destaque(ouro,3);
    g.DrawRectangle(&destaque,25+(origem.X-14)*2,115+(origem.Y-30)*2,origem.Width*2,origem.Height*2);
    texto(g,L"FOLHA: quadro destacado",520,120,18,ouro);
    texto(g,L"direcao = "+std::wstring(nomes[direcao]),520,150,17,claro);
    texto(g,L"origem = ("+std::to_wstring(origem.X)+L",38,"+std::to_wstring(origem.Width)+L",26)",520,178,17,claro);
    for (int l=0;l<5;++l) for (int c=0;c<7;++c) {
        Gdiplus::SolidBrush pincel(mapa[l][c]=='#'?Gdiplus::Color(255,95,105,122):Gdiplus::Color(255,49,75,73));
        g.FillRectangle(&pincel,42+c*55,275+l*55,53,53);
    }
    g.DrawRectangle(&destaque,42+logicaC*55,275+logicaL*55,53,53);
    Gdiplus::ImageAttributes atributos;
    atributos.SetColorKey(Gdiplus::Color(255,112,0,170),Gdiplus::Color(255,112,0,170));
    Gdiplus::Rect destino(42+int(visualC*55)+9,275+int(visualL*55)+1,36,52);
    g.DrawImage(folha.get(),destino,origem.X,origem.Y,origem.Width,origem.Height,Gdiplus::UnitPixel,&atributos);
    texto(g,L"AMARELO = celula logica reservada",480,320,18,ouro);
    texto(g,L"SPRITE = posicao visual interpolada",480,352,18,claro);
    texto(g,L"logica = ("+std::to_wstring(logicaL)+L","+std::to_wstring(logicaC)+L")",480,395,18,claro);
    wchar_t numeros[100]; swprintf(numeros,100,L"visual = (%.2f, %.2f)",double(visualL),double(visualC));
    texto(g,numeros,480,425,18,claro);
    texto(g,L"0,25 celula -> proxima pose",480,468,17,Gdiplus::Color(255,156,225,164));
    texto(g,L"ciclo: parado, A, parado, B",480,498,17,Gdiplus::Color(255,156,225,164));
    texto(g,L"Pare: pose 0. A parede central bloqueia.",42,585,17,claro);
}
LRESULT CALLBACK processar(HWND janela,UINT mensagem,WPARAM tecla,LPARAM dados) {
    if (mensagem==WM_TIMER) { atualizarMovimento(); InvalidateRect(janela,nullptr,FALSE); return 0; }
    if (mensagem==WM_KEYDOWN) {
        int dl=0,dc=0;
        if (tecla==VK_UP) { dl=-1; direcao=CIMA; }
        if (tecla==VK_DOWN) { dl=1; direcao=BAIXO; }
        if (tecla==VK_LEFT) { dc=-1; direcao=ESQUERDA; }
        if (tecla==VK_RIGHT) { dc=1; direcao=DIREITA; }
        int l=logicaL+dl,c=logicaC+dc;
        if (l>=0 && l<5 && c>=0 && c<7 && mapa[l][c]=='.') { logicaL=l; logicaC=c; }
        InvalidateRect(janela,nullptr,FALSE); return 0;
    }
    if (mensagem==WM_ERASEBKGND) return 1;
    if (mensagem==WM_PAINT) {
        PAINTSTRUCT pintura{}; HDC dc=BeginPaint(janela,&pintura);
        { Gdiplus::Bitmap buffer(LARGURA,ALTURA); Gdiplus::Graphics g(&buffer); desenharCena(g); Gdiplus::Graphics tela(dc); tela.DrawImage(&buffer,0,0); }
        EndPaint(janela,&pintura); return 0;
    }
    if (mensagem==WM_DESTROY) { KillTimer(janela,1); PostQuitMessage(0); return 0; }
    return DefWindowProcW(janela,mensagem,tecla,dados);
}
int WINAPI WinMain(HINSTANCE instancia,HINSTANCE,LPSTR,int exibir) {
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&tokenGdi,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    carregarFolha();
    WNDCLASSW classe{}; classe.lpfnWndProc=processar; classe.hInstance=instancia; classe.lpszClassName=L"Aula18Animacao";
    if (!RegisterClassW(&classe)) { folha.reset(); Gdiplus::GdiplusShutdown(tokenGdi); return 2; }
    RECT area{0,0,LARGURA,ALTURA}; DWORD estilo=WS_OVERLAPPEDWINDOW&~WS_THICKFRAME;
    AdjustWindowRect(&area,estilo,FALSE);
    HWND janela=CreateWindowExW(0,classe.lpszClassName,L"Animacao por distancia",estilo,CW_USEDEFAULT,CW_USEDEFAULT,area.right-area.left,area.bottom-area.top,nullptr,nullptr,instancia,nullptr);
    if (!janela) { folha.reset(); Gdiplus::GdiplusShutdown(tokenGdi); return 3; }
    ShowWindow(janela,exibir); SetTimer(janela,1,30,nullptr);
    MSG evento{}; while(GetMessageW(&evento,nullptr,0,0)>0) { TranslateMessage(&evento); DispatchMessageW(&evento); }
    folha.reset(); Gdiplus::GdiplusShutdown(tokenGdi); return int(evento.wParam);
}
