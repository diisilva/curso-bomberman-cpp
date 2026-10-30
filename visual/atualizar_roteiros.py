"""Alinha os comandos dos roteiros com os executaveis visuais principais."""
from pathlib import Path

raiz = Path(__file__).resolve().parents[1]
for pasta in (raiz / "aulas").iterdir():
    if not pasta.is_dir():
        continue
    n = int(pasta.name[5:7])
    roteiro = pasta / (pasta.name + ".md")
    if not roteiro.exists() or n == 5:
        continue
    texto = roteiro.read_text(encoding="utf-8")
    nome_exe = "Aula_01.exe" if n == 1 else pasta.name + ".exe"
    if n == 18:
        texto = texto.replace(r".\build\Aula_18_visual.exe", r".\build\internos\visuais\Aula_18_visual.exe")
        texto = texto.replace("O exemplo principal mostra o resultado da regra; `Aula_18_visual.cpp` permite observar quatro estados intermediários.",
                              "O programa principal já apresenta movimento real e permanece aberto. O diagrama complementar `Aula_18_visual.cpp` permite observar quatro estados da regra.")
    else:
        texto = texto.replace(" Para ver a saída do código original, use `ABRIR_AULAS.cmd` e escolha E (Exemplo).", "")
        texto = texto.replace(fr".\build\Aula_{n:02d}_visual.exe", fr".\build\{nome_exe}")
        texto = texto.replace("O exemplo principal mostra o resultado da regra;", "O executável principal abre a explicação visual; o exemplo de código mostra o resultado da regra.")
        texto = texto.replace("Tipo: console.", "Tipo: console no modo Exemplo; o executável principal abre uma janela visual.")
        if n not in (25, 26) and "Para executar o arquivo original" not in texto:
            frase = "Para executar o arquivo original `.cpp` e ler sua saída, abra `ABRIR_AULAS.cmd` e escolha **E (Exemplo)**.\n\n"
            texto = texto.replace("Use **setas**", frase + "Use **setas**", 1)
    roteiro.write_text(texto, encoding="utf-8")
print("Roteiros atualizados")
