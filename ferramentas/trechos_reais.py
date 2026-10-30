"""Preenche os blocos "O código real" dos roteiros com trechos de projeto_final/bomberman.cpp.

Cada roteiro marca um trecho com um comentário HTML seguido de um bloco ```cpp:

    <!-- trecho simbolo="lerTeclado" de="if (telaMenu==MENU_FASES)" ate="return;" -->
    ```cpp
    (conteúdo gerado: não edite à mão)
    ```

- simbolo: função, struct ou enum definido no início de uma linha do fonte.
- de / ate (opcionais): recortam o trecho dentro do símbolo. "de" escolhe a
  primeira linha que contém o texto; "ate" escolhe a primeira linha seguinte
  (inclusive) que contém o texto. Sem "simbolo", procuram no arquivo inteiro.
- max (opcional): limite de linhas; o padrão é 45 para manter o trecho legível.

Uso, na raiz do curso:
    python ferramentas/trechos_reais.py            # atualiza os roteiros
    python ferramentas/trechos_reais.py --conferir # só verifica (falha se algo mudou)

Como o trecho é sempre copiado do fonte, o número de linha e o código nunca
ficam desatualizados: basta rodar de novo depois de editar o jogo.
"""
from pathlib import Path
import re
import sys

RAIZ = Path(__file__).resolve().parents[1]
FONTE = RAIZ / "projeto_final" / "bomberman.cpp"
# O bloco pode estar vazio (recém-escrito); o conteúdo nunca atravessa outra cerca ```.
# Os valores entre aspas podem conter ">" (ex.: static_cast<int>).
MARCADOR = re.compile(r'<!-- trecho (?P<args>(?:"(?:[^"\\]|\\.)*"|[^">])*?)-->[ \t]*\n```cpp\n(?:(?:(?!```).)*?\n)?```', re.S)
ARG = re.compile(r'(\w+)="((?:[^"\\]|\\.)*)"')


def sem_strings(linha: str) -> str:
    """Remove literais e comentários para contar chaves com segurança."""
    linha = re.sub(r'"(?:[^"\\]|\\.)*"', '""', linha)
    linha = re.sub(r"'(?:[^'\\]|\\.)*'", "''", linha)
    return linha.split("//", 1)[0]


def localizar_simbolo(linhas, simbolo):
    padroes = [
        re.compile(rf'^(?:struct|enum|class)\s+{re.escape(simbolo)}\b'),
        re.compile(rf'^[A-Za-z_][\w:<>,\*&\s]*?[\s\*&]{re.escape(simbolo)}\s*\('),
        re.compile(rf'^template<[^>]*>\s*[\w:<>\*&\s]+?[\s\*&]{re.escape(simbolo)}\s*\('),
    ]
    for i, texto in enumerate(linhas):
        if not any(p.match(texto) for p in padroes):
            continue
        if texto.rstrip().endswith(";") and "{" not in texto:
            continue  # declaração antecipada, não definição
        profundidade, abriu = 0, False
        for j in range(i, len(linhas)):
            limpo = sem_strings(linhas[j])
            profundidade += limpo.count("{") - limpo.count("}")
            abriu = abriu or "{" in limpo
            if abriu and profundidade <= 0:
                return i, j
        raise ValueError(f"chaves sem fechamento em {simbolo}")
    raise ValueError(f"símbolo não encontrado: {simbolo}")


def extrair(linhas, args):
    inicio, fim = 0, len(linhas) - 1
    if "simbolo" in args:
        inicio, fim = localizar_simbolo(linhas, args["simbolo"])
    if "de" in args:
        alvo = args["de"]
        achado = next((i for i in range(inicio, fim + 1) if alvo in linhas[i]), None)
        if achado is None:
            raise ValueError(f'"de" não encontrado: {alvo}')
        inicio = achado
    if "ate" in args:
        alvo = args["ate"]
        achado = next((i for i in range(inicio, fim + 1) if alvo in linhas[i]), None)
        if achado is None:
            raise ValueError(f'"ate" não encontrado: {alvo}')
        fim = achado
    limite = int(args.get("max", 45))
    if fim - inicio + 1 > limite:
        raise ValueError(f"trecho com {fim - inicio + 1} linhas (máximo {limite}); use de/ate")
    return inicio, fim


def bloco(linhas, args):
    inicio, fim = extrair(linhas, args)
    nome = args.get("simbolo", "")
    faixa = f"linha {inicio + 1}" if inicio == fim else f"linhas {inicio + 1}–{fim + 1}"
    titulo = f"// projeto_final/bomberman.cpp, {faixa}"
    if nome:
        titulo += f" ({nome})"
    corpo = "\n".join(linhas[inicio:fim + 1]).rstrip()
    return f"{titulo}\n{corpo}"


def processar(roteiro: Path, linhas, conferir: bool):
    texto = roteiro.read_text(encoding="utf-8")
    erros = []

    def trocar(m):
        args = {k: v.replace('\\"', '"') for k, v in ARG.findall(m.group("args"))}
        try:
            codigo = bloco(linhas, args)
        except ValueError as erro:
            erros.append(f"{roteiro.relative_to(RAIZ)}: {erro}")
            return m.group(0)
        return f'<!-- trecho {m.group("args").strip()} -->\n```cpp\n{codigo}\n```'

    soltos = texto.count("<!-- trecho ") - len(MARCADOR.findall(texto))
    if soltos:
        erros.append(f"{roteiro.relative_to(RAIZ)}: {soltos} marcador(es) sem bloco ```cpp logo abaixo")
        return erros, False, 0
    novo = MARCADOR.sub(trocar, texto)
    mudou = novo != texto
    if mudou and not conferir:
        roteiro.write_text(novo, encoding="utf-8")
    return erros, mudou, len(MARCADOR.findall(texto))


def main():
    conferir = "--conferir" in sys.argv
    if not FONTE.exists():
        sys.exit("projeto_final/bomberman.cpp nao encontrado: rode baixar_jogo_final.ps1")
    linhas = FONTE.read_text(encoding="utf-8").splitlines()
    roteiros = sorted(RAIZ.glob("aulas/*/*.md")) + sorted(RAIZ.glob("*.md"))
    todos_erros, mudaram, total = [], [], 0
    for roteiro in roteiros:
        erros, mudou, quantidade = processar(roteiro, linhas, conferir)
        todos_erros += erros
        total += quantidade
        if mudou:
            mudaram.append(str(roteiro.relative_to(RAIZ)))
    for erro in todos_erros:
        print("ERRO", erro)
    if conferir and mudaram:
        for nome in mudaram:
            print("DESATUALIZADO", nome)
    print(f"{total} trechos reais em {len(roteiros)} arquivos; "
          f"{'desatualizados' if conferir else 'atualizados'}: {len(mudaram)}")
    sys.exit(1 if todos_erros or (conferir and mudaram) else 0)


if __name__ == "__main__":
    main()
