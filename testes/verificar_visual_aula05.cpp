#define WinMain Aula05WinMain
#include "../aulas/Aula_05_recorte_de_sprite_sheet/Aula_05_recorte_de_sprite_sheet.cpp"
#undef WinMain
#include <iostream>

int main() {
    Gdiplus::GdiplusStartupInput entrada;
    if (Gdiplus::GdiplusStartup(&token, &entrada, nullptr) != Gdiplus::Ok) return 1;
    criarFolhas();
    UINT quantidade = 0, bytes = 0;
    Gdiplus::GetImageEncodersSize(&quantidade, &bytes);
    std::vector<BYTE> memoria(bytes);
    auto* codificadores = reinterpret_cast<Gdiplus::ImageCodecInfo*>(memoria.data());
    Gdiplus::GetImageEncoders(quantidade, bytes, codificadores);
    CLSID png{};
    bool encontrou = false;
    for (UINT i = 0; i < quantidade; ++i) {
        if (wcscmp(codificadores[i].MimeType, L"image/png") == 0) {
            png = codificadores[i].Clsid;
            encontrou = true;
            break;
        }
    }
    if (!encontrou) return 2;
    for (size_t i = 0; i < folhas.size(); ++i) {
        if (!folhas[i].imagem) { std::cout << "asset ausente " << i << "\n"; return 3; }
        folhaAtual = i;
        modoFolhaInteira = true;
        indiceNaFolha = 0;
        andando = true;
        corChaveLigada = true;
        Gdiplus::Bitmap captura(LARGURA_JANELA, ALTURA_JANELA);
        {
            Gdiplus::Graphics g(&captura);
            desenharCena(g);
        }
        std::wstring arquivo = L"aulas\\Aula_05_recorte_de_sprite_sheet\\imagens\\aula05_folha_" + std::to_wstring(i + 1) + L".png";
        if (captura.Save(arquivo.c_str(), &png, nullptr) != Gdiplus::Ok) return 4;
        std::cout << "captura " << i + 1 << " salva\n";
    }
    folhaAtual = 2;
    modoFolhaInteira = false;
    direcao = DIREITA;
    passoDoCiclo = 1;
    corChaveLigada = false;
    {
    Gdiplus::Bitmap captura(LARGURA_JANELA, ALTURA_JANELA);
    {
        Gdiplus::Graphics g(&captura);
        desenharCena(g);
    }
    if (captura.Save(L"aulas\\Aula_05_recorte_de_sprite_sheet\\imagens\\aula05_sem_cor_chave.png", &png, nullptr) != Gdiplus::Ok) return 5;
    }
    folhas.clear();
    Gdiplus::GdiplusShutdown(token);
    return 0;
}
