# Plano do curso

Aluno: conhece lógica de programação, começa C++ e jogos Windows. Percurso: 33 aulas visuais em 11 trilhas, 27 diagramas de quatro etapas como executável principal, 3 marcos jogáveis e 33 soluções demonstrativas. A Aula 05 é um explorador visual completo e serve de referência de método para as outras.

## Trilhas

As aulas foram numeradas na ordem em que foram escritas; a sequência de estudo é a das trilhas. Cada roteiro diz, na primeira tabela, a trilha, o que estudar antes e a próxima aula.

| Trilha | Aulas | Marco observável |
| --- | --- | --- |
| Fundamentos de C++ | 01 → 02 → 29 | posição e mapa em console; o C++ do jogo real (templates, lambdas, ponteiros inteligentes) |
| Janela e desenho | 03 → 04 → 05 → 24 | janela, formas, recorte de sprite, backbuffer e DPI |
| Tempo, movimento e animação | 06 → 07 → 18 | acumulador, interpolação, colisão e animação direcional |
| Bombas e poderes | 08 → 09 → 19 | chama recursiva, oito itens e o motor completo dos poderes |
| IA e busca | 10 → 11 → 20 | BFS, previsão da explosão e fuga segura do bot |
| Campanha e cenário | 12 → 27 | fases, dificuldade, dois jogadores e as dez arenas |
| Ranking e arquivos | 13 → 21 → 32 | CSV simples, CSV robusto com `.tmp`/`.bak` e a tela de ranking com renomear e excluir |
| Chefes e aliado | 14 → 22 → 25 → 26 | projéteis, máquina de estados, Mario e os cinco chefes reais |
| Áudio | 15 → 23 | amostras PCM e arquivo WAV completo |
| Entrada e menus | 16 → 31 → 33 → 28 → 30 | camada de entrada, joystick real, quem usa cada controle, rota do menu e menu completo |
| Integração | 17 | leitura guiada do jogo completo (fim do curso) |

Uma ordem possível para quem começa do zero: as primeiras aulas de cada trilha (01–17, na ordem numérica), depois as continuações de cada trilha. Os três marcos de `marcos/` fazem a ponte jogável após as Aulas 07, 09 e 12.

Estude cada exemplo antes da função correspondente no projeto final. Os roteiros trazem os trechos reais do jogo com o número da linha, copiados automaticamente por `ferramentas/trechos_reais.py`.

Depois das trilhas, use `ARQUITETURA_DA_RECONSTRUCAO.md` para ligar os módulos e execute `DESAFIO_DE_RECONSTRUCAO.md` em uma pasta nova: onze etapas ligam todos os sistemas e exigem evidência antes da consulta ao fonte real.

Os marcos cobrem mapa, bombas, adversário simples e duas fases. Os sistemas avançados aparecem nas continuações das trilhas (Aulas 18–33); as Aulas 25 e 26 usam recortes e casos do Mario e dos cinco chefes reais, as Aulas 27 e 28 retomam o jogo original para cenários e fluxo do menu, e as Aulas 30–33 constroem as telas de menu, ranking e controles. O jogo final não faz parte do curso: está no [GitHub](https://github.com/diisilva/bomberman-univali-prog-2) e `baixar_jogo_final.ps1` o baixa para `projeto_final/`, na versão contra a qual os roteiros foram conferidos.
