# Curso de Bomberman em C++17 para Windows

Uma trilha prática de **33 aulas em 11 trilhas**, com programas visuais, 33 soluções e três marcos jogáveis. Você começa pelos fundamentos de C++ e chega a janelas Win32, desenho com GDI+, animação, busca em largura, ranking, áudio e controles, estudando o [jogo Bomberman UNIVALI](https://github.com/diisilva/bomberman-univali-prog-2) como referência.

## Comece aqui

1. Prepare o ambiente indicado em [Requisitos e instalação](#requisitos-e-instalação).
2. Baixe o jogo de referência com `.\baixar_jogo_final.ps1` na raiz do curso.
3. Escolha uma trilha no [mapa das aulas](#mapa-das-aulas) e abra o roteiro pelo link.
4. Leia a explicação, rode o exemplo, faça a atividade e só então compare com a solução.
5. Compile a aula com `.\compilar_aula.ps1 -Numero 1` (troque o número) ou use `ABRIR_AULAS.cmd` para abrir os programas já compilados.
6. Ao concluir as trilhas, jogue os [três marcos](./marcos/README.md) e faça o [desafio de reconstrução](./DESAFIO_DE_RECONSTRUCAO.md).

Os números indicam a ordem em que as aulas foram criadas. **Dentro do curso, siga a sequência de cada trilha**; a Aula 17 fecha a leitura integrada do jogo. Cada roteiro informa pré-requisitos, tempo estimado, prática e próxima aula.

## Mapa das aulas

### Fundamentos de C++

Modele posições, mapas e os recursos da linguagem usados no jogo.

| Aula | Prática |
| --- | --- |
| [01 — Tipos, funções e referências](./aulas/Aula_01_tipos_funcões_e_referências/Aula_01_tipos_funcões_e_referências.md) | Estruture posição com tipos, funções e referências. |
| [02 — Matriz, enum e mapa](./aulas/Aula_02_matriz_enum_e_mapa/Aula_02_matriz_enum_e_mapa.md) | Represente arena em matriz e classifique células com enum. |
| [29 — O C++ que o jogo usa](./aulas/Aula_29_cpp_que_o_jogo_usa/Aula_29_cpp_que_o_jogo_usa.md) | Reconheça o C++ usado pelo código real. |

### Janela e desenho

Abra uma janela, desenhe a interface, recorte sprites e cuide do backbuffer e da escala.

| Aula | Prática |
| --- | --- |
| [03 — Janela e mensagens Win32](./aulas/Aula_03_janela_e_mensagens_win32/Aula_03_janela_e_mensagens_win32.md) | Abra a janela e trate mensagens Win32. |
| [04 — Formas, cores e GDI+](./aulas/Aula_04_formas_cores_e_gdiplus/Aula_04_formas_cores_e_gdiplus.md) | Desenhe formas, texto e cores com GDI+. |
| [05 — O recorte que vira animação](./aulas/Aula_05_recorte_de_sprite_sheet/Aula_05_recorte_de_sprite_sheet.md) | Explore o recorte de folhas de sprites. |
| [24 — Backbuffer, escala e DPI](./aulas/Aula_24_backbuffer_dpi_recursos/Aula_24_backbuffer_dpi_recursos.md) | Desenhe em backbuffer e adapte escala e DPI. |

### Tempo, movimento e animação

Faça a atualização por tempo, trate movimento e colisões e anime personagens por direção.

| Aula | Prática |
| --- | --- |
| [06 — Tempo, entrada e animação](./aulas/Aula_06_tempo_entrada_e_animacao/Aula_06_tempo_entrada_e_animacao.md) | Atualize entrada e quadros pelo tempo. |
| [07 — Movimento e colisão](./aulas/Aula_07_movimento_e_colisao/Aula_07_movimento_e_colisao.md) | Mova personagens e resolva colisões. |
| [18 — Animação direcional pela distância](./aulas/Aula_18_animacao_direcional_gdiplus/Aula_18_animacao_direcional_gdiplus.md) | Recorte e anime personagens por direção e distância. |

### Bombas e poderes

Implemente explosões, os oito poderes e a lógica completa de seus efeitos.

| Aula | Prática |
| --- | --- |
| [08 — Bomba e explosão recursiva](./aulas/Aula_08_bomba_e_explosao_recursiva/Aula_08_bomba_e_explosao_recursiva.md) | Propague a explosão e aplique suas regras. |
| [09 — Os oito poderes](./aulas/Aula_09_oito_poderes/Aula_09_oito_poderes.md) | Modele os oito poderes do jogo. |
| [19 — Motor completo dos poderes](./aulas/Aula_19_motor_completo_dos_poderes/Aula_19_motor_completo_dos_poderes.md) | Combine poderes e efeitos em um motor completo. |

### IA e busca

Use fila e BFS para o bot escolher caminhos e fugir do perigo.

| Aula | Prática |
| --- | --- |
| [10 — Fila e busca em largura (BFS)](./aulas/Aula_10_fila_e_bfs/Aula_10_fila_e_bfs.md) | Encontre rotas com fila e BFS. |
| [11 — Perigo e fuga do bot](./aulas/Aula_11_perigo_e_fuga_do_bot/Aula_11_perigo_e_fuga_do_bot.md) | Marque perigos e mova o bot para a fuga. |
| [20 — BFS de fuga segura: o bot só planta onde consegue fugir](./aulas/Aula_20_bfs_fuga_segura/Aula_20_bfs_fuga_segura.md) | Confirme uma rota segura antes de plantar bombas. |

### Campanha e cenário

Monte fases, dificuldades, dois jogadores e arenas a partir de matrizes e tiles.

| Aula | Prática |
| --- | --- |
| [12 — Fases, dificuldade e dois jogadores](./aulas/Aula_12_fases_dificuldade_e_cooperativo/Aula_12_fases_dificuldade_e_cooperativo.md) | Monte fases e opções para um ou dois jogadores. |
| [27 — Cenários: da matriz à arena desenhada](./aulas/Aula_27_cenarios_tabuleiro_e_tiles/Aula_27_cenarios_tabuleiro_e_tiles.md) | Transforme matriz e tiles em cenários jogáveis. |

### Ranking e arquivos

Grave pontuações em CSV e construa a tela para consultar e editar o ranking.

| Aula | Prática |
| --- | --- |
| [13 — Ranking em CSV](./aulas/Aula_13_ranking_csv/Aula_13_ranking_csv.md) | Grave e leia pontuações em CSV. |
| [21 — Ranking CSV completo](./aulas/Aula_21_ranking_csv_completo/Aula_21_ranking_csv_completo.md) | Valide, ordene e persista o ranking completo. |
| [32 — A tela de ranking: listar, renomear e excluir](./aulas/Aula_32_tela_de_ranking_crud/Aula_32_tela_de_ranking_crud.md) | Liste, renomeie e exclua entradas do ranking. |

### Chefes e aliado

Explore projéteis, estados de chefes e o aliado Mario no jogo de referência.

| Aula | Prática |
| --- | --- |
| [14 — Chefes, projéteis e Mario: os números da luta](./aulas/Aula_14_chefes_projeteis_e_mario/Aula_14_chefes_projeteis_e_mario.md) | Calcule ataques, projéteis e ações de Mario. |
| [22 — Máquina de estados do chefe](./aulas/Aula_22_maquina_de_estados_boss/Aula_22_maquina_de_estados_boss.md) | Modele os estados e transições de um chefe. |
| [25 — Mario aliado: movimento e maçã](./aulas/Aula_25_mario_aliado_e_maca/Aula_25_mario_aliado_e_maca.md) | Anime Mario aliado e a maçã na arena. |
| [26 — Os cinco chefes reais da campanha](./aulas/Aula_26_cinco_chefes_reais/Aula_26_cinco_chefes_reais.md) | Examine os cinco chefes da campanha. |

### Áudio

Entenda amostras PCM, gere um WAV e reproduza os sons.

| Aula | Prática |
| --- | --- |
| [15 — Áudio: amostras PCM](./aulas/Aula_15_audio_wav_e_recursos/Aula_15_audio_wav_e_recursos.md) | Gere amostras PCM para um efeito sonoro. |
| [23 — Arquivo WAV completo e reprodução](./aulas/Aula_23_wav_pcm_completo/Aula_23_wav_pcm_completo.md) | Escreva e reproduza um arquivo WAV. |

### Entrada e menus

Trate teclado e joystick, configure controles e conecte o menu à partida.

| Aula | Prática |
| --- | --- |
| [16 — Controle e camada de entrada](./aulas/Aula_16_controle_e_camada_de_entrada/Aula_16_controle_e_camada_de_entrada.md) | Unifique teclado e controle em uma camada de entrada. |
| [31 — Conectando um joystick de verdade](./aulas/Aula_31_joystick_real/Aula_31_joystick_real.md) | Leia a entrada de um joystick conectado. |
| [33 — Configurando os controles: quem joga com o quê](./aulas/Aula_33_configuracao_de_controles/Aula_33_configuracao_de_controles.md) | Configure a associação entre jogadores e controles. |
| [28 — Como o menu conduz à partida](./aulas/Aula_28_menu_e_transicoes_reais/Aula_28_menu_e_transicoes_reais.md) | Acompanhe as transições do menu à partida. |
| [30 — Construindo um menu completo](./aulas/Aula_30_menu_completo/Aula_30_menu_completo.md) | Construa um menu interativo completo. |

### Integração

Leia os sistemas do jogo real com um roteiro guiado.

| Aula | Prática |
| --- | --- |
| [17 — Leitura guiada do jogo real](./aulas/Aula_17_leitura_orientada_do_jogo_real/Aula_17_leitura_orientada_do_jogo_real.md) | Localize no jogo real os sistemas estudados. |
## Requisitos e instalação

- Windows 10 ou 11, PowerShell e Git.
- MSYS2 UCRT64 com `g++` no `PATH` para compilar C++17.
- Python para conferir os trechos do jogo real citados nas aulas.

Na raiz do curso, execute:

```powershell
g++ --version
.\baixar_jogo_final.ps1
.\compilar_aula.ps1 -Numero 1
.\build\Aula_01.exe
```

O download coloca o jogo de referência em `projeto_final/`. Essa pasta é ignorada pelo Git do curso. As aulas que usam sprites e as ferramentas que conferem trechos do código precisam desse download. O script usa por padrão o commit `bf2fb5ec09d21b05de8ba29b2c91143e0f22b148`, correspondente aos trechos e números de linha dos roteiros.

Para escolher uma aula em um menu, abra `ABRIR_AULAS.cmd`. As opções **Visual**, **Exemplo** e **Solução** permitem estudar o mesmo tópico em etapas. No PowerShell, você também pode usar `.\abrir_aula.ps1 -Numero 32 -Modo Exemplo`.

## Marcos e projeto de reconstrução

Os [marcos jogáveis](./marcos/README.md) mostram a evolução do mapa até uma campanha curta. Depois, use a [arquitetura](./ARQUITETURA_DA_RECONSTRUCAO.md) e as onze etapas do [desafio de reconstrução](./DESAFIO_DE_RECONSTRUCAO.md) para montar seu próprio jogo em outra pasta. O [guia dos sistemas](./GUIA_DOS_SISTEMAS.md), o [mapa do projeto](./MAPA_DO_PROJETO.md) e a [leitura orientada](./LEITURA_ORIENTADA.md) ajudam a relacionar as aulas ao código de referência.

## Como verificar o curso

```powershell
.\validar_aulas.ps1
.\compilar_solucoes.ps1
.\compilar_marcos.ps1
.\validar_regras.ps1
```

`validar_aulas.ps1` compila as aulas, executa exemplos de console, confere saídas e capturas e compara os trechos citados nos roteiros com o jogo baixado. Veja [VALIDACAO.md](./VALIDACAO.md) para o procedimento e [PROGRESSO.md](./PROGRESSO.md) para os resultados registrados. Os programas gráficos e o áudio ainda pedem inspeção manual.

## O que está neste repositório

- [`aulas/`](./aulas/): roteiros, fontes, soluções e capturas de cada aula.
- [`visual/`](./visual/): código de apoio para as apresentações visuais.
- [`testes/`](./testes/): verificações automatizadas das aulas e dos marcos.
- [`ferramentas/`](./ferramentas/): geração de diagramas e conferência de trechos do jogo.
- [`imagens_jogo_real/`](./imagens_jogo_real/): capturas de telas usadas para explicar o jogo de referência.
- [Plano do curso](./PLANO_DO_CURSO.md) e [cobertura](./COBERTURA_DO_CURSO.md): sequência, dependências e sistemas estudados.

Os executáveis, arquivos temporários e o jogo baixado são gerados localmente e ficam fora do versionamento do curso.

## Código e imagens de terceiros

O jogo final e seus recursos são obtidos do [repositório de referência](https://github.com/diisilva/bomberman-univali-prog-2). Sprites e personagens de jogos pertencem a seus respectivos titulares. As imagens de estudo presentes neste curso devem ter a origem e a permissão de redistribuição conferidas antes de torná-lo público.

## Próxima ação

Instale o ambiente, execute a Aula 01 e marque o que conseguiu reproduzir. Ao terminar cada trilha, volte aos marcos e observe quais sistemas novos você já consegue explicar.