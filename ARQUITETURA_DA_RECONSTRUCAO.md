# Arquitetura para reconstruir o Bomberman

Este é o caderno de integração para usar **depois** dos exemplos pequenos. Abra `DESAFIO_DE_RECONSTRUCAO.md` para os critérios das onze etapas. Implemente em uma pasta nova, sem colar `projeto_final/bomberman.cpp`. Use o jogo copiado como gabarito de comportamento após testar cada etapa.

## Uma decisão antes de cada função

Escolha e escreva a unidade de cada valor. `linha` e `coluna` são células inteiras; `visualL` e `visualC` são células fracionárias; origem e destino de sprite são pixels; simulação e fusível usam milissegundos; velocidade de projétil usa células por segundo. Uma conversão implícita entre essas unidades é um bug provável. As Aulas 05, 06, 07, 14 e 18 exibem essas conversões na tela.

Separe quatro responsabilidades:

```text
teclado / controle -> Entrada -> atualizar(Estado, dt) -> Estado novo
                                      |                    |
                                      +-> eventos de som    +-> desenhar(Estado)
Estado e resultado final -> ranking CSV
```

`desenhar` consulta o estado e não decide colisão, coleta nem dano. `atualizar` não abre PNG ou arquivo CSV a cada tick. A mesma função de regra deve servir para teste sem janela e para partida com janela.

## Contratos mínimos entre arquivos

| Arquivo sugerido | Deve possuir | Exercite com |
| --- | --- | --- |
| `estado.h` | `Posicao`, `TipoCelula`, `Jogador`, `Bomba`, `Item`, `Inimigo`, `Boss`, `Partida` | Aulas 01–02, 09, 12 |
| `entrada.cpp` | Traduz teclas e controle para `Entrada {dl, dc, colocarBomba, usarPoder}` | Aulas 03, 16–17 |
| `regras.cpp` | Movimento, colisão, fusível, propagação, coleta, dano, fases | Aulas 06–12, 19–20, 27 |
| `ia.cpp` | BFS, predecessores, perigo e fuga | Aulas 10–11, 20 |
| `render.cpp` | Backbuffer, grade, sprite, animação, HUD e menus | Aulas 04–05, 18, 24, 27–28 |
| `recursos.cpp` | Vida de PNG, GDI+, áudio e handles | Aulas 05, 15, 23–24 |
| `ranking.cpp` | Ler, validar, ordenar e gravar arquivo temporário | Aulas 13, 21 |
| `main.cpp` | `WinMain`, criação, loop, mensagens e encerramento | Aulas 03, 06, 17, 24 |

Esta é uma divisão possível, não uma obrigação de nomes. Antes de integrar dois arquivos, desenhe a seta de dados entre eles e decida quem cria e quem libera cada recurso. Um `Bitmap` deve sobreviver aos quadros desenhados, mas ser destruído antes de `GdiplusShutdown`. A Aula 05 mostra isso no código real do visualizador.

## Ordem de integração que mantém o jogo executável

1. **Janela e grade:** desenhe o mapa 13 × 15 com paredes externas e pilares. Clique ou aperte tecla para alterar uma célula. Verifique índices nas quatro bordas.
2. **Jogador:** aceite direção apenas se a célula for livre. Guarde posição lógica separada da visual. Desenhe o alvo reservado e o corpo em trânsito como na Aula 18.
3. **Bomba:** crie fusível, quatro raios e persistência temporária da chama. Teste duas caixas consecutivas e uma parede sólida. O resultado de console da Aula 08 é `#.***#` na linha central do seu mapa de ensaio.
4. **Poderes:** mantenha as oito cartas em um único estado. Para cada carta, teste coletar, usar ou aplicar efeito, limite, consumo e reset. As Aulas 09 e 19 dão casos concretos.
5. **Inimigos e bot:** use a mesma função de célula transitável para jogador e busca, com parâmetros explícitos para poderes. Compare previsão de chama e chama real; a Aula 11 expõe a divergência quando há caixas.
6. **Campanha e dois jogadores:** separe dados que persistem entre fases de bônus que zeram. Teste modo de um e de dois jogadores antes de adicionar telas. A Aula 12 e `GUIA_DOS_SISTEMAS.md` especificam a transição.
7. **Ranking, áudio e chefes:** integre um sistema por vez. Grave CSV em arquivo de teste. Teste boss por estados e projéteis em subpassos; teste o WAV gerado antes de reproduzi-lo.
8. **Menus e recursos finais:** conecte fases, escala/DPI, menus, Modo Insano e liberação de recursos. Rode o build em pasta limpa e repita regressões.

Em cada passo, mantenha uma versão que abre, roda e fecha. Os três marcos de `marcos/` mostram versões menores até a campanha curta; as Aulas 18–26 detalham sistemas que o terceiro marco ainda não contém. Use as Aulas 25–26 para construir Mario e os cinco chefes após o boss genérico da Aula 22.

## Cinco verificações numéricas antes de abrir a janela

| Sistema | Entrada fixa | Saída a explicar |
| --- | --- | --- |
| Tempo | eventos de 16, 16, 40 e 16 ms; passo de 8 ms | 2, 4, 9 e 11 atualizações acumuladas |
| Movimento | origem (1,1), destino (1,2), interpolação de meia célula | lógico (1,2), visual 1,5; parede em (2,2) não muda o destino |
| Explosão | bomba em (2,1), duas caixas em (2,2) e (2,3), raio 4 | chama passa pelas caixas; parede sólida encerra |
| Busca | origem (1,1), alvo (1,5), corredor inferior | primeiro passo (2,1) |
| Fuga | bomba (3,3), raio 2, máximo 1 ou 4 passos | sem fuga em 1; fuga em 2 passos com limite 4 |

Se alguma saída não bater, conserte a regra pequena antes de conectá-la à tela. O script `validar_aulas.ps1` compila e executa os exemplos do curso; os testes em `projeto_final/testes/` verificam o jogo de referência, não a sua. Reproduza esses cenários na sua própria implementação.

## Checklist de autonomia

Você está pronto para tentar o jogo sem consultar o fonte quando consegue explicar: onde nasce uma ação de teclado e como vira movimento; por que posição lógica e visual diferem; como um `Rect` da folha chega ao `DrawImage`; em que ordem fusível, chama, dano e coleta alteram o estado; como a BFS reconstrói caminho e fuga; o que persiste entre fases; e quem libera PNG, WAV, HDC, bitmap e DLL. Se um desses pontos ainda depende de decorar uma linha, volte à aula correspondente e refaça o exercício com valores novos.
