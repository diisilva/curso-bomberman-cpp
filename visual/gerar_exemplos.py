"""Gera os diagramas de quatro etapas (Aula_NN_visual.cpp) a partir de roteiros_visuais.json.

Os textos de cada etapa ficam no JSON; este script só monta o C++. Para mudar
um diagrama, edite o JSON e rode, na raiz do curso:

    python visual/gerar_exemplos.py            # cria apenas os que faltam
    python visual/gerar_exemplos.py --force    # recria todos

Formato de cada aula no JSON:
    titulo, tipo ("fluxo" | "grade" | "barras"), contexto (frase "No jogo: ..."),
    etapas: [{item, conta, resultado, porque, foco:[l,c]?, mapa:[...]?}] x4,
    mapa (grade com o mesmo mapa nas quatro etapas), valores (barras),
    itens (true para mostrar os oito ícones de itens reais).
"""
from pathlib import Path
import json
import sys

ROOT = Path(__file__).resolve().parents[1]
DADOS = json.loads((Path(__file__).with_name("roteiros_visuais.json")).read_text(encoding="utf-8"))


def wide(valor):
    return "L" + json.dumps(valor, ensure_ascii=False)


def fonte(n, aula):
    titulo, tipo, etapas = aula["titulo"], aula["tipo"], aula["etapas"]
    assert len(etapas) == 4, f"aula {n}: são necessárias quatro etapas"
    linhas_etapas = []
    for i, e in enumerate(etapas):
        l, c = e.get("foco", [-1, -1]) if tipo == "grade" else (-1, -1)
        linhas_etapas.append("    {" + ", ".join([wide(f"Etapa {i+1}: {e['item']}"), wide(e["conta"]), wide(e["resultado"]),
                                                  wide(e["porque"]), str(i), str(l), str(c)]) + "}")
    por_etapa = tipo == "grade" and "mapa" in etapas[0]
    codigo_mapa = ""
    if tipo == "grade":
        if por_etapa:
            codigo_mapa = "".join(f"const char* mapa{i}[] = {{{', '.join(json.dumps(x) for x in e['mapa'])}}};\n"
                                  for i, e in enumerate(etapas))
            codigo_mapa += "const char* const* mapas[4] = {mapa0,mapa1,mapa2,mapa3};\n"
            grade = etapas[0]["mapa"]
        else:
            grade = aula["mapa"]
            codigo_mapa = "const char* mapa[] = {" + ", ".join(json.dumps(x) for x in grade) + "};\n"
        argumentos_mapa = f"{'mapas[passo]' if por_etapa else 'mapa'}, {len(grade)}, {len(grade[0])}"
    else:
        argumentos_mapa = "nullptr, 0, 0"
    codigo_valores = ""
    if tipo == "barras":
        codigo_valores = "const int valores[4] = {" + ", ".join(str(x) for x in aula["valores"]) + "};\n"
    valores = "valores" if tipo == "barras" else "nullptr"
    icones = "    visual::mostrarItens(g);" if aula.get("itens") else ""
    nome = f"Aula {n:02d}: {titulo}"
    return f'''// Aula {n:02d}: diagrama interativo gerado por visual/gerar_exemplos.py (edite roteiros_visuais.json).
// Setas percorrem a regra; 2 esconde a resposta; 1 revela; Espaço anima.
#include "../../visual/visual_base.h"
const wchar_t* itens[4] = {{{", ".join(wide(e["item"]) for e in etapas)}}};
const visual::Etapa etapas[4] = {{
{",\n".join(linhas_etapas)}
}};
{codigo_mapa}{codigo_valores}void desenharAula(Gdiplus::Graphics& g, int passo, int variante) {{
    visual::passo=passo;
    visual::variante=variante;
    visual::explicar(g, passo, variante, visual::Tipo::{tipo.title()}, itens, etapas, {argumentos_mapa}, {valores}, {wide(aula["contexto"])});
{icones}
}}
#ifndef CAPTURA
int WINAPI WinMain(HINSTANCE instancia, HINSTANCE, LPSTR, int exibir) {{
    return visual::executar(instancia, exibir, {wide(nome)}, desenharAula);
}}
#else
int main() {{
    Gdiplus::GdiplusStartupInput entrada;
    ULONG_PTR token=0;
    if (Gdiplus::GdiplusStartup(&token,&entrada,nullptr)!=Gdiplus::Ok) return 1;
    visual::titulo={wide(nome)};
    bool ok=true;
    for (int passo : {{0,1,2,3}}) {{
        Gdiplus::Bitmap imagem(visual::LARGURA, visual::ALTURA);
        Gdiplus::Graphics g(&imagem);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        desenharAula(g,passo,0);
        wchar_t caminho[80];
        swprintf(caminho,80,L"imagens\\\\etapa_%d.png",passo+1);
        if (!visual::salvarPng(imagem,caminho)) ok=false;
    }}
    visual::liberarAssets();
    Gdiplus::GdiplusShutdown(token);
    return ok?0:2;
}}
#endif
'''


def main():
    pastas = {int(p.name[5:7]): p for p in (ROOT / "aulas").iterdir() if p.is_dir()}
    gerados = 0
    for chave, aula in DADOS.items():
        n = int(chave)
        destino = pastas[n] / f"Aula_{n:02d}_visual.cpp"
        if destino.exists() and "--force" not in sys.argv:
            continue
        destino.write_text(fonte(n, aula), encoding="utf-8")
        (pastas[n] / "imagens").mkdir(exist_ok=True)
        gerados += 1
    print(f"Gerados {gerados} diagramas; use --force para recriar os existentes")


if __name__ == "__main__":
    main()
