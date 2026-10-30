# Validação

Ambiente de referência: Windows, PowerShell, g++ MSYS2 UCRT64, C++17.

## Critérios

- Compilação: comando termina com código zero; avisos são registrados.
- Execução de console: saída e código de retorno observados.
- Interface gráfica: `validar_aulas.ps1` exporta PNGs e testa estados; interação da janela em máquina do aluno é uma verificação complementar.
- Controle físico: leitura de XInput sem dispositivo não prova direção, botões nem desconexão.

## Roteiro manual

1. Abra aulas 03–05; confira janela, bola translúcida e quadro de sprite. Feche sem erro.
2. Abra `projeto_final/bomberman.exe`; percorra menu, nomes, dificuldade e dez fases.
3. Em fase com duas caixas frágeis consecutivas, confronte a chama real com a previsão do bot.
4. Colete cada item; teste teclas E, T e F, alcance, limite de bombas, imunidade própria e travessia de caixa.
5. Teste dois jogadores nos modos amigos e rivais; compare vidas, pontos e bombas individuais.
6. Conecte, desconecte e reconecte um controle; confira zona morta e foco da janela.
7. Feche o jogo e confira encerramento de áudio e processo do Modo Insano.

Para repetir o teste automatizado das aulas, execute `.\validar_aulas.ps1` na raiz. Ele confere os 171 trechos reais copiados nos roteiros, compila 33 executáveis visuais principais, os exemplos e 33 soluções, executa 21 exemplos de console, confere suas saídas, roda os testes sem janela das Aulas 05, 18, 30, 32 e 33 e os três marcos. `compilar_aula.ps1 -Numero N` compila a aula e confere se as capturas diferem. Evidências da execução realizada estão em PROGRESSO.md.

Para abrir uma aula sem perder a saída de console, use `ABRIR_AULAS.cmd` ou `abrir_aula.ps1`. Os 27 executáveis em `build/internos/exemplos/` também aceitam duplo clique: os programas de console aguardam Enter, e os gráficos (Aulas 03, 04, 24, 30, 32 e 33) ficam abertos até fechar. Binários de captura e teste ficam em outras pastas de `build/internos/` e encerram sozinhos após produzir resultados.

`validar_abertura.ps1` inicia ocultos os 33 executáveis principais e os 27 exemplos, confirma que os 60 processos permanecem ativos após a inicialização e encerra apenas esses processos de teste. Isso detecta falha imediata ao abrir, enquanto as capturas verificam o desenho separadamente.

## Resultados registrados em 2026-10-30

A revisão foi executada em uma cópia temporária do estado de trabalho do curso, no Windows com PowerShell e MSYS2 UCRT64. Nenhum arquivo gerado pelos testes foi gravado no repositório do curso.

| Verificação | Resultado |
| --- | --- |
| `ferramentas/trechos_reais.py --conferir` | 171 trechos em 44 arquivos, nenhum desatualizado |
| `validar_aulas.ps1` | 33 aulas e 33 soluções compiladas; capturas e verificações das Aulas 05, 18, 30, 32 e 33 passaram |
| Exemplos de console | 21 executados com saída esperada |
| Marcos 1–3 | Verificações automatizadas passaram |
| Abertura das janelas | 60 executáveis iniciaram sem falha imediata |
| `validar_regras.ps1` | Seis regressões do jogo de referência passaram |
| Download do jogo de referência | `git fetch --depth 1` do commit `bf2fb5ec09d21b05de8ba29b2c91143e0f22b148` passou em repositório novo |

Foram inspecionadas capturas das Aulas 01 e 33. A reprodução audível do WAV, o uso de um joystick físico e a jogabilidade do jogo final ainda exigem conferência manual.
## Resultados registrados em 2026-10-29

| Verificação | Resultado |
| --- | --- |
| `ferramentas/trechos_reais.py --conferir` | 171 trechos em 43 roteiros, nenhum desatualizado |
| `compilar_aula.ps1` para 1–33 | 33 executáveis principais compilaram; 32 aulas exportaram capturas com estados distintos (a Aula 05 não gera capturas pelo script) |
| `compilar_solucoes.ps1` | 33 soluções compilaram |
| exemplos de console | 21 executados com código zero e saídas esperadas (Aulas 01, 02, 06–17, 19–23, 29 e 31) |
| `verificar_aula05`, `verificar_aula18`, `verificar_aula30` | passaram |
| `verificar_aula32` | 25 comportamentos do CRUD do ranking; quatro telas exportadas e inspecionadas |
| `verificar_aula33` | 23 comportamentos da configuração de controles; cinco telas exportadas e inspecionadas |
| marcos 1–3 | verificados |
| `validar_abertura.ps1` | não executado nesta rodada (abre 60 janelas ocultas); repetir numa máquina livre |

Correções desta revisão: a Aula 30 não compilava pelo script porque faltava o diagrama `Aula_30_visual.cpp`, embora o roteiro já o citasse; ele foi gerado a partir de `roteiros_visuais.json`. Três cópias soltas `marcosmarco_*.png` na raiz, idênticas às de `marcos/`, foram removidas.

Observação de ambiente: os executáveis usam o `libstdc++-6.dll` do MSYS2. Rodados a partir do Git Bash, que põe o `mingw64/bin` do Git antes no PATH, os exemplos com `fstream` (como as Aulas 13 e 32) quebram com violação de acesso. Rode os scripts pelo PowerShell ou com `C:\msys64\ucrt64\bin` no início do PATH.

## Resultados registrados em 2026-10-13

| Verificação | Resultado |
| --- | --- |
| `compilar_todas.ps1` | 28 executáveis de aula e seus exemplos compilaram; 64 capturas exportadas e verificadas; as verificações das Aulas 05 e 18 exportam mais seis |
| `compilar_solucoes.ps1` | 28 soluções compilaram; as Aulas 27 e 28 incluem variações de densidade do mapa e seleção inicial |
| `validar_aulas.ps1 -SemCompilar` | 19 exemplos originais de console executados com código zero e saídas esperadas em `build/internos/exemplos/` |
| Aula 05 | três folhas reais e cor chave renderizadas em quatro PNGs; imagens inspecionadas |
| Aula 18 | recortes reais, interpolação e retorno à pose parada testados; duas capturas inspecionadas |
| `validar_abertura.ps1` | 28 executáveis principais e 22 exemplos originais permaneceram abertos no teste oculto |
| Aulas 27–28 | tabuleiro, 10 temas, menu e rota até a fase 10 testados; nove capturas exportadas e inspecionadas, incluindo a galeria das arenas |
| Aulas 25–26 | Mario, maçã e os cinco chefes reais renderizados; estados e PNGs distintos |
| `compilar_marcos.ps1` | três marcos gráficos compilaram; verificação visual pendente |
| Console 01, 02, 06–17 e 19–23 | executados com código de saída 0; valores principais conferidos no script |
| `projeto_final/compilar.ps1` | criou `bomberman.exe` e `modo_insano.exe` em `projeto_final/` |
| `verificar_colisoes` | passou: separação, cruzamento, chamas e P2 |
| `verificar_ranking_pontuacao` | passou: recordes, nomes, cursor, pontos e bônus |
| `verificar_controle` | passou: traduções simuladas, 1P, 2P, menu, fallback e bombas |
| `verificar_novos_bosses` | passou: Hornet e Bowser, ataques, contato e morte |
| `verificar_mario_aliado` | passou: resgate, movimento, maçãs, dano e vidas |
| `verificar_insano` | passou: combate, colisões e boss do modo Insano |

Os binários de regressão foram compilados e executados em `projeto_final/` (commit `bf2fb5e`). A reprodução audível do WAV, os três marcos e o jogo final ainda exigem inspeção manual da interface. Não havia controle físico conectado; o exemplo da Aula 16 cobriu a ausência do aparelho.
