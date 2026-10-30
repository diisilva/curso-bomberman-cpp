# Desafio final: refaça o Bomberman sem copiar o fonte

Objetivo: após as 33 aulas e os três marcos, construir em uma pasta nova um jogo C++17 para Windows que reproduza os sistemas observados no projeto de referência. Trabalhe com `projeto_final/` fechado durante a implementação de cada etapa. Consulte `MAPA_DO_PROJETO.md` e `GUIA_DOS_SISTEMAS.md` para requisitos; abra o fonte real somente depois de seu protótipo passar os critérios da etapa e registre o que descobriu de diferente. O jogo final copiado é uma solução de comparação e um alvo de fidelidade, não um arquivo para colar.

## Organização sugerida pelo aluno

Crie um novo diretório `minha_reconstrucao/` com `src/main.cpp`, `src/jogo.h`, `src/jogo.cpp`, `src/render.cpp`, `src/entrada.cpp`, `src/ranking.cpp`, `assets/`, `testes/` e `compilar.ps1`. Você pode escolher outra divisão, mas toda regra deve ter um dono claro. Comece com tipos pequenos: `Posicao`, `TipoCelula`, `EstadoJogo`, `Bomba`, `Item`, `Inimigo`, `Boss` e `RegistroPontuacao`. Use `std::vector` para entidades variáveis e `std::unique_ptr` ou outro RAII para recursos gráficos; explique quem libera cada handle Win32/GDI.

Antes de codificar uma função, escreva quatro frases: o que recebe, o que devolve, o que altera e qual cenário reproduzível mostra seu acerto. Guarde uma semente fixa no modo de testes e uma semente variável no jogo normal. Mantenha o compilador em C++17 com `-Wall -Wextra -pedantic`.

## Etapas obrigatórias e critérios de passagem

| Etapa | O aluno implementa | Evidência antes de avançar |
| --- | --- | --- |
| 1. Janela e tempo | `WinMain`, classe, callback, mensagens, fechamento, `WM_TIMER`, acumulador com subpasso de 8 ms e limite de atraso | janela abre/fecha; simulação igual após eventos de 16 e 32 ms que somem o mesmo tempo; recursos liberados |
| 2. Desenho | GDI+ com formas e textos, backbuffer GDI, cache básico, carregamento de PNG relativo ao executável, recorte 16 × 24 e quadro de caminhada | personagem percorre quatro direções e três frames; fundo não pisca ao redimensionar; asset ausente gera diagnóstico |
| 3. Grade e corpo | mapa 13 × 15, borda, pilares, caixas, posição lógica/visual, cooldown, teclado contínuo, ordem das direções e foco | nunca sai da matriz; não atravessa paredes; curva espera fim do passo; `WM_KILLFOCUS` para movimento |
| 4. Bomba e itens | bomba com fusível, explosão recursiva, caixa bônus oculta, oito itens, limite por dono, chute, teleporte, relógio, imunidade e travessia | duas caixas consecutivas quebram com raio suficiente; cada item tem caso de coleta, consumo, limite e reset |
| 5. Inimigos e bot | patrulha, perseguição, BFS com predecessores, perigo real, bomba hipotética, busca de fuga e escolha de alvo | bot encontra caminho ao redor de obstáculo; não planta em beco sem saída; caso de duas caixas demonstra diferença entre previsão e chama |
| 6. Campanha | dez fases, dificuldade, inimigos com bombas, portal, `montarFase`, `reiniciarFase`, `avancarFase`, `novaPartida` | transições preservam e zeram exatamente os estados previstos; fases pares esperam chefe; portal só vale após chegada visual |
| 7. Dois jogadores | posição, direção, animação, vidas individuais, limite de bombas por dono, recursos compartilhados e modos amigos/rivais | teclado P1/P2 funciona; dano e pontos diferem conforme modo; morte de um jogador não congela o outro |
| 8. Menus e ranking | estados de menu, teclado/mouse, escala de clique, UTF-8, CSV de cinco campos, aspas, ordenação, melhor recorde, edição, exclusão, temporário e backup | arquivo de teste reabre nome com `;` e `"`; empate usa bombas/movimentos; ranking só grava vitória ou fim de partida manual |
| 9. Chefes e Mario | cinco chefes nas fases pares, aviso/ataque/recuperação/morte, imunidade, projéteis com subpassos, atlas Hornet/Bowser, aliado sob caixa e maçãs | cada chefe chega a morte; portal aguarda animação; Mario só surge em fase par e interage com inimigos e boss |
| 10. Controle e áudio | camada `EntradaControle`, SDL2/XInput/DirectInput conforme disponibilidade, zona morta, desconexão, WAV RIFF/PCM em memória e desligamento | teclado e controle geram as mesmas ações; desconexão não cria movimento fantasma; áudio para antes de liberar buffer |
| 11. Integração | DPI, redimensionamento, menus completos, modo automático, Modo Insano externo, mensagens de falha e liberação final | build limpo em pasta nova; dez fases jogáveis; fechar Insano retorna ao menu; sem handles ou imagens pendurados |

Não trate os itens 6–11 como um único último passo. Faça um commit local após cada etapa. Entre etapas, rode os testes da sua própria implementação e compare com os testes de `projeto_final/testes`. Esses testes incluem o fonte de referência e por isso não validam automaticamente o código que você escreveu; adapte cenários, entradas e expectativas, sem copiar a implementação.

## Sequência de avaliação autônoma

1. **Previsão:** antes de rodar, registre saída esperada com valores numéricos. Exemplo: raio 2, duas caixas seguidas, quantas células acendem?
2. **Experimento:** rode em mapa e semente fixos; salve saída, screenshot ou log de estado.
3. **Comparação:** abra apenas a função correspondente do projeto real; anote comportamento igual, diferença pedagógica ou erro do seu programa.
4. **Correção:** corrija sem apagar o cenário que revelou o problema.
5. **Regressão:** execute novamente casos antigos e o novo caso.

Para regras discretas, faça testes sem janela: explosão, coleta, reset, BFS, pontuação, CSV e transições de chefe. Para desenho e hardware, use roteiro manual: sprites, escala, foco, áudio e joystick real. Registro `compilou` é diferente de `executei`, `vi na tela` e `testei com controle`.

## Entrega que demonstra autonomia

O aluno entrega seu executável, fonte, assets com origem preservada, script de build, testes, cinco screenshots de fases distintas, um CSV de teste e `DECISOES.md` com: arquitetura, unidades de tempo/posição, donos dos recursos, três bugs encontrados, diferenças em relação ao original e como adicionaria um nono poder. Na apresentação, implementa uma pequena regra nova sem consultar o fonte de referência: por exemplo, uma carta que reduz o fusível de bombas futuras com teto mínimo e reset de fase, acompanhada de um teste.

Considere o objetivo atingido quando o aluno consegue explicar o fluxo completo `entrada → atualização → regras → desenho → liberação`, localizar e modificar cada sistema, completar as onze etapas, compilar em uma pasta nova e demonstrar o jogo. O código de referência permanece disponível como gabarito de comparação após cada etapa.
