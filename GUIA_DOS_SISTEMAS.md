# Guia de observação dos sistemas reais

Use este guia depois de executar os exemplos. As descrições abaixo se referem ao código baixado para `projeto_final/` por `baixar_jogo_final.ps1` e indicam o que procurar; os exemplos curtos simplificam várias regras. Não altere `ranking.csv` do original.

## Janela, tempo e coordenadas

`WinMain` cria a janela, inicializa GDI+ e entra em `GetMessageW`/`DispatchMessageW`. `processarMensagem` recebe entrada, timer, pintura, tamanho, DPI e destruição. `SetTimer(janela, 1, PASSO_LOGICA_MS, nullptr)` solicita eventos com intervalo de 16 ms. Em `WM_TIMER`, o código mede tempo real desde o evento anterior, soma no acumulador com limite de 100 ms e chama `atualizar(8)` enquanto houver pelo menos 8 ms. Portanto, 16 ms é intervalo solicitado para o timer e 8 ms é o passo da simulação; não são a mesma coisa. Temporizadores de janela dependem da entrega das mensagens, não representam relógio de alta precisão.

O teclado mantém `teclaPressionada`; `WM_KEYDOWN` marca a tecla e registra sua ordem no primeiro acionamento; `WM_KEYUP` desmarca; `WM_KILLFOCUS` limpa o estado para impedir movimento preso. A ordem dá prioridade à direção pressionada mais recentemente. Ações pontuais passam por `lerTeclado`. O clique do mouse converte coordenadas físicas pela margem e pela escala antes de testar cartões de menu.

Uma célula lógica `Posicao{linha,coluna}` pode ser reservada antes do desenho chegar a ela. `avancarVisual` desloca coordenadas `float` gradualmente; colisão e coleta são verificadas usando a posição visual ou depois de `visualNoDestino`, conforme a regra. Experimento: no depurador, acompanhe `jogador.coluna` e `jogadorVisualC` durante um único movimento.

GDI+ desenha formas, textos e PNGs com transparência no DC do buffer. `garantirBuffer` prepara o backbuffer; GDI o apresenta com `BitBlt`. Caches de mapa, fontes e sprites reduzidos são criados sob demanda; `invalidarCacheMapa` é chamado quando caixas ou fase mudam. Ao redimensionar, `ajustarAreaDesenho` preserva a proporção; `WM_DPICHANGED` recalcula a escala. Faça a revisão visual com tamanho normal, janela maximizada e escala de monitor diferente antes de afirmar que o layout está correto.

## Folhas e animações

O jogador direcional usa recortes de 16 × 24 pixels, três quadros por direção e quatro linhas na ordem baixo, direita, cima, esquerda. A posição de origem é `x = quadro × 16`, `y = linhaDireção × 24`; o destino pode ter tamanho diferente, produzindo escala. Quatro direções são quatro vistas discretas; mover por ângulo ou animar oito vistas requer atlas e regras adicionais. Hornet e Bowser usam tabelas `ANIM_HORNET` e `ANIM_BOWSER` de sequências de comprimentos diferentes; aplicar a grade 3 × 4 a esses atlas estaria errado.

## Mapa, fases e dificuldade

A grade da campanha tem 13 linhas e 15 colunas. `criarMapa` cria bordas, pilares e caixas. Bloco especial `PAREDE_BONUS` é desenhado igual a `PAREDE_FRAGIL`; o item aparece quando `destruirBlocoBonus` o remove. `montarFase` reconstitui mapa, spawns, inimigos, boss, Mario, bombas, itens e bônus da fase. `reiniciarFase` chama `montarFase` e preserva pontos, estatísticas acumuladas e vidas globais restantes. `avancarFase` incrementa fase, repõe vidas básicas, zera contadores de eficiência da fase e monta. `novaPartida` recomeça pontos, estatísticas, vidas e fase selecionada. Em 2P há vidas individuais por fase e a regra de equipe ou rivalidade altera pontuação e dano.

As fases pares 2, 4, 6, 8 e 10 têm chefe. O portal exige limpar inimigos e concluir o chefe quando houver. A dificuldade muda quantidade base de inimigos (3, 5, 7), intervalos e perseguição; a seleção de inimigos que soltam bomba é 1 no Fácil, cerca de metade no Médio e 75% no Difícil. Para estudar aleatoriedade, fixe temporariamente uma semente somente na cópia do curso e compare o mapa gerado.

## Oito itens e bombas

Todos os poderes ficam ocultos em blocos bônus, aparecem após a destruição e são coletados quando o personagem alcança a célula. `coletarItensEm` remove o item do vetor, acrescenta 30 pontos e altera o estado. `montarFase` limpa os bônus, exceto as vidas globais que são responsabilidade dos chamadores. A tabela resume o efeito, mas a condição de uso deve ser lida nas funções citadas.

| Item | Estado alterado | Uso e limite no código |
| --- | --- | --- |
| CHUTE | `possuiCartaChute` | ação E/X; chuta uma bomba à frente até quatro células livres e consome a carta após chute válido |
| TELEPORTE | `possuiCartaTeleporte` | ação T/Y; escolhe célula livre sem inimigo/bomba/P2, sincroniza posição visual, coleta eventual item e consome a carta |
| RAIO | `alcanceExplosao` | efeito imediato, cresce de 1 até 5; sem ação separada |
| BOMBA_EXTRA | `bombasExtras` | efeito imediato; limite simultâneo por dono é `1 + bombasExtras` |
| VIDA_EXTRA | `vidas` | efeito imediato nas vidas globais; não é a mesma variável das vidas individuais 2P por fase |
| BOMBA_RELOGIO | `temBombaRelogio` | bombas passam a esperar detonação por F ou L/R; no modo rivais a bomba de P2 usa temporizador comum |
| IMUNE_BOMBA | `imuneAPropriaBomba` | protege apenas da própria bomba conforme dono da chama; não protege de bomba alheia |
| ATRAVESSA_BLOCO | `atravessaBlocoQuebravel` | permite entrar em `PAREDE_FRAGIL`; não inclui `PAREDE_BONUS` nem `PAREDE_SOLIDA` |

`atualizarBombas` reduz temporizadores, inicia a chama, acompanha sua duração e remove bombas terminadas. `propagarExplosao` tem caso base por alcance e barreira sólida; atravessa e destrói caixas frágeis consecutivas até consumir o alcance. Já `celulaNoAlcance` para a previsão na primeira caixa; com `#.++..#`, origem na coluna 1 e alvo na 3, a chama real chega ao segundo `+`, mas a previsão retorna falso. A aula 11 expõe essa divergência para análise, sem mudar a regra no jogo final.

## Inimigos, bot e controle

`moverInimigos` usa movimento aleatório e perseguição conforme dificuldade e distância. `bfsProximoPasso` usa fila, células visitadas, predecessores e reconstrução do primeiro passo. O bot avalia perigo, objetivos e uma fuga após bomba hipotética por `existeFugaSegura`. É IA baseada em regras e busca, sem aprendizado automático. O exemplo 10 ensina a fila antes do template/lambdas usados na função real.

`EntradaControle` separa ações do jogo da fonte física. SDL2 é carregada da pasta do executável para controles mapeados; XInput é alternativa para Xbox compatível; DirectInput genérico cobre outros dispositivos com ressalvas do código. Na aula 16, o eixo horizontal vira -1/0/1 após uma zona morta, e A vira bomba. No jogo, confirme o foco da janela, conexão e desconexão; quando um dispositivo sai, o teclado pode reassumir. Um teste sem hardware só valida tradução simulada.

## Ranking e fim de partida

Cada linha de `ranking.csv` contém cinco campos separados por ponto e vírgula: data, jogador, pontuação, bombas e movimentos. `lerCamposCsv` trata aspas duplicadas; `lerRanking` valida inteiros não negativos, consolida nomes e ordena. O desempate prefere mais pontos, depois menos bombas, menos movimentos e então chave do nome. `gravarRanking` tenta backup `.bak`, escreve `.tmp` e usa `MoveFileExA`; backup pode falhar sem bloquear a escrita, logo não há garantia absoluta de preservação. Os nomes são transformados para UTF-8; examine `caminhoRankingUtf8` e as APIs A para distinguir codificação do conteúdo e dos caminhos.

Uma derrota de fase gasta vida; ranking é salvo somente quando uma partida manual chega a vitória ou fim de jogo. Os pontos são ganhos por inimigo (100), caixa (20), item (30), boss (1000) e bônus de portal `max(0, 500 - movimentosFase - 5 × bombasFase)`. Andar e plantar não descontam imediatamente. O exemplo 13 usa CSV simples, sem parser robusto; a implementação real está na cópia final.

## Chefes, aliado, áudio e processo externo

Golem, Egg-HVC, Hornet, Guardião Arcano e Bowser aparecem nas fases pares. `Boss` guarda vida, imunidade e temporizadores; `EtapaHornet` discrimina fases de ação; projéteis têm posição, velocidade, vida e subpassos para não atravessar alvos. `MarioAliado` é encontrado sob caixa comum em fase de chefe, desloca-se, foge e lança maçãs. A Aula 14 isola posição, velocidade e vida; as Aulas 25 e 26 mostram os sprites, tempos e poderes do jogo real, cujo combate completo está em `projeto_final/`.

`gerarMusicaProcedural` monta um WAV RIFF/PCM em memória com onda quadrada e envelope. `PlaySoundW` usa o buffer em loop; ele deve sobreviver à reprodução, e `pararMusica` precede a liberação. A aula 15 gera somente PCM para mostrar amostras; cabeçalho, envelope e reprodução ficam para leitura na fonte real.

`iniciarModoInsano` cria outro processo e acompanha seu handle para voltar ao menu. O script de build gera `batalha_portavel.cpp` com caminho de asset relativo. Um lançamento bem-sucedido depende de `modo_insano.exe` e da árvore de assets ao lado de `bomberman.exe`.
