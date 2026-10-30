"""Acrescenta estudo guiado das capturas sem alterar a Aula 05 do professor."""
from pathlib import Path
import sys
from gerar_exemplos import DATA, ROOT

DESAFIOS = {
 1:"Troque `Posicao&` por `Posicao` e explique por que `jogador` deixa de mudar. Depois restaure a referencia.",
 2:"Acrescente uma segunda caixa em (1,4). Faca o mapa textual e a visualizacao concordarem.",
 3:"Trate `WM_KEYDOWN`: ao apertar Espaco, mude o titulo da janela com `SetWindowTextW`.",
 4:"Desenhe um segundo circulo com alfa diferente e explique a composicao sobre o fundo.",
 6:"Experimente eventos de 7, 9 e 31 ms. Mostre o resto acumulado e prove que nenhum milissegundo se perde.",
 7:"Introduza `visualL` e interpole tambem o movimento vertical. Teste uma parede e um corredor.",
 8:"Coloque uma parede solida apos a primeira caixa. Compare mapa antes e depois da explosao.",
 9:"Faça um teste para cada carta, incluindo limites de raio e reset ao iniciar outra fase.",
 10:"Mostre a fila apos cada retirada e reconstrua a rota completa usando `anterior`.",
 11:"Unifique previsao e explosao numa unica regra de alcance; teste duas caixas consecutivas.",
 12:"Implemente tres fases e confira que pontos persistem enquanto bonus temporarios zeram.",
 13:"Adicione empate por movimentos e verifique a ordem reabrindo o CSV gerado.",
 14:"Teste um projetil que atravessaria um alvo em um tick longo e reduza o tamanho do subpasso.",
 15:"Gere quatro ondas de frequencias distintas e calcule quantos bytes cada buffer consome.",
 16:"Traduza teclado e controle para a mesma `Entrada`; teste sem controle conectado.",
 17:"No projeto final, anote a funcao concreta chamada em cada uma das quatro etapas do diagrama.",
 18:"Troque o limiar da pose de 0,25 para 0,50 celula. Preveja quantas vezes a pose muda ao andar uma celula.",
 19:"Crie casos com chute bloqueado, teleporte sem destino e imunidade contra bomba de outro jogador.",
 20:"Mude a caixa e o raio; confira se a fuga em dois passos continua segura e cabe no prazo.",
 21:"Teste nome com ponto e virgula, aspas e linha vazia; reabra o arquivo e compare os cinco campos.",
 22:"Simule ticks de 100 ms, conte as transicoes e prove que o boss derrotado nao dispara.",
 23:"Abra `som_aula.wav` em modo binario e confira 44 bytes de cabecalho mais 4000 de dados.",
 24:"Redimensione a janela e compare o tamanho do circulo em DPI 96 e 144; confira liberacao do bitmap.",
}

folders={int(p.name[5:7]):p for p in (ROOT/"aulas").iterdir() if p.is_dir()}
atualizados=0
for n,(titulo,modo,nodes,rows,grid,extra) in DATA.items():
    folder=folders[n]
    doc=folder/(folder.name+".md")
    before=doc.read_text(encoding="utf-8")
    if "<!-- INICIO_VISUAL -->" in before and "--force" not in sys.argv:
        continue  # Roteiros editados pelo professor/aluno nao sao sobrescritos.
    if n==18:
        before="""# Aula 18 — Animação direcional pela distância\n\n**Duração:** 2 a 3 horas. **Pré requisitos:** aulas 05–07 e 17.\n\nA folha real `personagem_principal_sprite_v2.png` é irregular. A tabela de `Rect` desta aula usa os mesmos recortes medidos na Aula 05, nossa referência para a escolha da origem e da cor chave.\n\n![Folha, quadro destacado, célula lógica e sprite em movimento](imagens/aula18_movimento.png)\n\nO programa usa setas para reservar uma célula livre. `logicaL/C` muda imediatamente; `visualL/C` se aproxima 0,08 célula por timer. Cada 0,25 célula percorrida avança o ciclo `{0,1,0,2}`. Quando as posições coincidem, `indiceCiclo` volta a zero. A parede central bloqueia a entrada.\n\n## Construção\n\n1. Releia `criarPersonagemFolhaIrregular` da Aula 05 e confira `quadroX`, `quadroLargura`, Y=38 e altura=26 desta aula. A origem não pode ser `quadro*16`.\n2. Siga `WM_KEYDOWN`: a direção é escolhida e `mapa[l][c]=='.'` decide se a célula será reservada.\n3. Siga `WM_TIMER` até `atualizarMovimento`: anote célula lógica, posição visual, distância acumulada e índice do ciclo a cada chamada.\n4. Siga `WM_PAINT` até `desenharCena`: o contorno da folha usa a origem; o contorno amarelo no mapa usa a posição lógica; o sprite usa a visual.\n5. Confira `ImageAttributes::SetColorKey`: RGB(112,0,170) remove o roxo somente do sprite recortado.\n\nCompile com `.\\compilar_aula.ps1 -Numero 18` e execute `.\\build\\Aula_18_animacao_direcional_gdiplus.exe` **da raiz do curso**. Use as quatro setas, tente atravessar a parede e pare após mover. Compare sua previsão com os números na tela.\n\n![Estado de repouso antes do movimento](imagens/aula18_inicio.png)\n\nA solução muda o limiar de 0,25 para 0,50 célula; a pose troca com metade da frequência espacial. O teste `testes/verificar_aula18.cpp` exporta as duas capturas e verifica recorte, interpolação e pose de repouso. Compare depois com `linhaDaFolhaParaDirecao`, `desenharQuadroNaCelula` e `quadroCaminhada` do jogo final.\n\n"""
    start=before.find("<!-- INICIO_VISUAL -->")
    if start>=0:
        before=before[:start].rstrip()+"\n"
    rows_md=[]
    for i,row in enumerate(rows):
        conta,resultado,explicacao=row.split("|",2)
        rows_md.append(f"| {i+1}. {nodes[i]} | `{conta}` | {resultado} | {explicacao} |")
    section=f"""
<!-- INICIO_VISUAL -->
## Laboratório visual: preveja, avance e confira

![Primeira etapa da {titulo.lower()}](imagens/etapa_1.png)

O exemplo principal mostra o resultado da regra; `Aula_{n:02d}_visual.cpp` permite observar quatro estados intermediários. O código visual fica nesta pasta e usa `visual/visual_base.h` somente para janela, desenho e controles. Os dados e as contas de cada etapa estão no arquivo da aula.

```powershell
.\\compilar_aula.ps1 -Numero {n}
.\\build\\Aula_{n:02d}_visual.exe
```

Use **setas** para andar pelas etapas; **2** esconde a conta e o resultado para você prever; **1** revela; **Espaço** anima ou pausa. Antes de avançar, diga o valor esperado e a razão. Se o visual e o programa principal divergirem, investigue o código.

| Etapa | Conta ou condição | Resultado que deve aparecer | Por que importa |
| --- | --- | --- | --- |
{chr(10).join(rows_md)}

![Última etapa da {titulo.lower()}](imagens/etapa_4.png)

### Faça você mesmo

{DESAFIOS[n]} Execute o exemplo original e a variação; registre entrada, previsão e resultado. Depois identifique no jogo final onde essa regra reaparece. As capturas são produzidas novamente por `compilar_aula.ps1`; se faltarem, a compilação informa o erro.
"""
    doc.write_text(before.rstrip()+"\n\n"+section,encoding="utf-8")
    atualizados+=1
print(f"Roteiros visuais atualizados: {atualizados}; use --force para recriar os existentes")
