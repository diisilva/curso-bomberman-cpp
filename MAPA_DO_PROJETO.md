# Mapa do projeto

Fonte observada: commit `bf2fb5e` (2026-10-13) do [GitHub](https://github.com/diisilva/bomberman-univali-prog-2), baixado por `baixar_jogo_final.ps1`. As faixas de linha são pontos de partida, pois podem mudar em edições futuras.

| Sistema | Arquivos e símbolos → comportamento observado | Conceitos | Aulas | Verificação | Classificação |
| --- | --- | --- | --- | --- | --- |
| Win32 e desenho | bomberman.cpp:3986-4035, 4260-4450 — ajustarAreaDesenho, desenhar, processarMensagem, WinMain | janela, evento, GDI, GDI+ | 03, 04, 05, 17 | compilação; abrir e fechar janela | Implementado no original; exemplo é adaptação pedagógica |
| Estado e C++ | bomberman.cpp:40-190 — TipoCelula, TipoItem, Posicao, Bomba, Boss | constante, enum, struct, vetor | 01, 02, 09, 17 | saídas de 01 e 02 | Implementado no original; exemplo é adaptação pedagógica |
| Sprites e animação | bomberman.cpp:2914-2950 — linhaDaFolhaParaDirecao, desenharQuadroNaCelula | folha, frame, recorte 16 × 24, 3 × 4 | 05, 06, 18 | compilação; inspeção visual pendente | Implementado no original; exemplo é adaptação pedagógica |
| Tempo e entrada | bomberman.cpp:2396-2522, 4260-4410 — atualizar, WM_TIMER | timer 16 ms, subpasso 8 ms, teclado contínuo | 06, 07, 16 | saída de 06; jogo visual pendente | Implementado no original; exemplo é adaptação pedagógica |
| Mapa e campanha | bomberman.cpp:1050-1327, 2980-3050 — criarMapa, montarFase, temaDaFase, desenharTerrenoCelula, avancarFase | matriz 13 × 15, dez fases, oito spawns, tiles reais | 02, 12, 27 | saída de 12; quatro capturas de 27; regressão em testes | Implementado no original; exemplo é adaptação pedagógica |
| Movimento e contato | bomberman.cpp:852-881, 1352-1408 — avancarVisual, moverJogador | destino lógico, interpolação, colisão | 07 | saída de 07; verificar_colisoes | Implementado no original; exemplo é adaptação pedagógica |
| Bombas e explosão | bomberman.cpp:1409-1523, 1593-1633 — Bomba, propagarExplosao, atualizarBombas | recursão, tempo, blocos, chama | 08, 19 | saída de 08; regressão em testes | Implementado no original; exemplo é adaptação pedagógica |
| Oito itens | bomberman.cpp:105, 1328-1347 — TipoItem, coletarItensEm | coleta, efeito, reset | 09, 19 | saídas de 09 e 19 | Implementado no original; exemplo é adaptação pedagógica |
| Inimigos e bot | bomberman.cpp:1634-1779, 2157-2395 — bfsProximoPasso, existeFugaSegura, atualizarBot | fila, BFS, perigo, discrepância de alcance | 10, 11, 20 | saídas de 10, 11 e 20 | Implementado no original; exemplo é adaptação pedagógica |
| Cooperativo e controle | bomberman.cpp:249-299, 424-628, 913-949 — EntradaControle, atualizarEstadoControle | dois jogadores, SDL2, XInput, DirectInput | 12, 16, 31, 33 | leitura sem hardware; verificar_controle | Implementado no original; exemplo é adaptação pedagógica |
| Menus e ranking | bomberman.cpp:232-243, 684-842, 3605-4180 — TelaMenu, desenharMenu, desenharSeletorFases, lerTeclado, lerRanking, gravarRanking | estados, navegação, CSV de cinco campos, Unicode | 13, 17, 21, 28, 30, 32 | saídas de 13 e 21; quatro capturas e rota de 28; verificar_ranking_pontuacao; verificar_aula30 e verificar_aula32 | Implementado no original; exemplo é adaptação pedagógica |
| Chefes e Mario | bomberman.cpp:153-174, 301-328, 1847-2142, 2250-2332 — Boss, EtapaHornet, MarioAliado | projétil, subpasso, IA do aliado | 14, 22, 25, 26 | saídas de 14 e 22; capturas de Mario e dos cinco chefes; regressão em testes | Exemplos gráficos usam sprites e regras reais; integração completa no jogo final |
| Áudio e recursos | bomberman.cpp:2722-2827, 3999-4035 — gerarMusicaProcedural, garantirBuffer | WAV PCM, GDI backbuffer, RAII | 15, 23, 24 | saída de 15; jogo visual pendente | Implementado no original; exemplo é adaptação pedagógica |
| DPI e modo Insano | bomberman.cpp:3986-3998, 4037-4054, 4248-4259 — iniciarModoInsano | escala, processo filho, handles | 17, 24 | build do projeto final | Implementado no original; exemplo é adaptação pedagógica |

## Correções ao enunciado e dependências

- Controle já está implementado no fonte atual. SDL2 é preferida; XInput e DirectInput são alternativas carregadas conforme disponibilidade. A aula 16 isola XInput, mas não constitui uma extensão inédita.
- O segundo jogador possui posição e direção próprias. A versão atual tem modos amigos e inimigos, vidas por jogador e limite de bombas por dono; pontos podem ser separados no modo rivais. Assim, a descrição de recursos inteiramente compartilhados no prompt não vale para todos os modos.
- `propagarExplosao` atravessa caixas frágeis consecutivas. `celulaNoAlcance` interrompe a previsão na primeira caixa. Aula 11 reproduz essa divergência; essa versão do jogo não a corrige.
- O fonte e executável do Modo Insano estão disponíveis. `compilar.ps1` gera uma versão portátil do fonte de referência.
- `bosses_animacoes.h`, `assets/`, `assets_otimizados/`, `PLANO_M2.md` e scripts de preparação existem no original. Todos vêm junto quando o jogo é baixado.
- A extensão solicitada de joystick já existia na versão encontrada. Não foi necessário inventar sua implementação.
