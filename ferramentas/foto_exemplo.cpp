// Fotografa um exemplo gráfico sem abrir janela.
//
// Os exemplos de janela do curso separam o desenho numa função
//     void desenharCena(Gdiplus::Graphics& g);
// e declaram LARGURA e ALTURA. Este arquivo inclui o exemplo, troca o nome
// do WinMain dele (para não haver dois pontos de entrada) e chama
// desenharCena num Bitmap em memória. Uso (feito por ferramentas/fotografar_exemplos.ps1):
//     g++ -DEXEMPLO="\"caminho/do/exemplo.cpp\"" ferramentas/foto_exemplo.cpp -lgdiplus
//     a.exe saida.png
#define WinMain WinMain_do_exemplo
#include EXEMPLO
#undef WinMain
#include <cstring>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) return 2;
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token = 0;
    if (Gdiplus::GdiplusStartup(&token, &entrada, nullptr) != Gdiplus::Ok) return 1;
    bool ok = false;
    {
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
        std::wstring caminho(argv[1], argv[1] + std::strlen(argv[1]));
        for (UINT i = 0; i < quantidade; ++i)
            if (std::wcscmp(codecs[i].MimeType, L"image/png") == 0)
                ok = imagem.Save(caminho.c_str(), &codecs[i].Clsid, nullptr) == Gdiplus::Ok;
    }
    Gdiplus::GdiplusShutdown(token);
    return ok ? 0 : 3;
}
