# Cobertura e diferenças

| Funcionalidade | Aula | Exemplo | Diferença da versão didática | Verificação |
| --- | --- | --- | --- | --- |
| C++ e matriz | 01–02 | posição e mapa 5 × 7 | simplificação temporária do mapa 13 × 15 | execução |
| Janela, GDI+ e sprites | 03–05 | janela, formas, três folhas e recorte interativo | Aula 05 percorre grade regular, bomba e folha irregular com cor chave | compila; quatro capturas da Aula 05 verificadas |
| Tempo e colisão | 06–07 | acumulador e posição visual | console em lugar de mensagens reais | execução |
| Bombas e oito itens | 08–09 | chama e estado | sem animação ou coleta no mapa | execução |
| BFS, bot e perigo | 10–11 | busca e caso de divergência | busca reduzida; bot completo no final | execução |
| Fases, dificuldade, 2P | 12 | transição de partida | somente estado mínimo | execução |
| Ranking e CSV | 13 | CSV de cinco campos | sem aspas, Unicode ou backup; edição e exclusão ficam para a Aula 32 | execução |
| Chefes e Mario | 14, 22, 25–26 | projétil, HP, Mario aliado e cinco encontros reais | aulas 25–26 mostram recortes e regras reais; integração completa permanece no jogo final | execução e capturas |
| Música, recursos e DPI | 15, 17, 23–24 | PCM, RIFF/WAV e backbuffer | áudio audível e DPI de outro monitor exigem verificação manual | execução, captura didática |
| Controle | 16 | XInput por DLL | SDL2 e DirectInput na Aula 31 | compilação, sem hardware |
| Jogo completo | 17, projeto_final | campanha e Modo Insano no GitHub | fonte de referência sem comentários em cada linha | build |
| Ponte jogável | marcos 1–3 | jogo_incremental.cpp | GDI, duas fases e regras reduzidas; sem sistemas avançados | compilação |
| Sprite direcional e tempo | 18 | recorte irregular da Aula 05, interpolação e animação por distância | usa uma folha e um mapa de ensaio | compilação; duas capturas e estado de repouso testados |
| Poderes com ações | 19 | motor de oito itens | teleporte determinístico e sem mapa completo | execução |
| BFS de fuga | 20 | mapa com bomba e limite de passos | uma bomba, sem inimigos móveis | execução |
| CSV robusto | 21 | escape, leitura, validação de inteiros, recorde e backup | sem limpeza de nome nem chave sem maiúsculas (Aula 32) | execução |
| Chefe e aliado | 22 | estados, vetor de direção, projétil e dano | um boss genérico e colisão como desafio | execução |
| WAV completo | 23 | RIFF/PCM e reprodução | usa arquivo síncrono em vez do loop em memória | execução; 4044 bytes verificados; audição pendente |
| Backbuffer e DPI | 24 | GDI+ em bitmap e BitBlt | buffer recriado por pintura | compilação; diagrama capturado; DPI real pendente |
| Mario, maçã e chefes reais | 25–26 | sprite real, movimento e voo; cinco retratos, vidas e poderes | janelas didáticas isolam os sistemas; campanha completa no jogo final | duas capturas Mario e sete da galeria; janelas abertas |
| Tabuleiro e dez cenários reais | 27 | matriz 13 × 15, 36% de caixas, oito spawns, oito bônus e recortes reais de dez arenas | fase 6 mostra os recortes base; o original recolore pedra e caixa | quatro etapas, galeria comparativa e invariantes da montagem |
| Menu e seletor de fases | 28 | rota Jogar/IA/atalho, estados, dez cards e preview real do Casino Azul | nomes e dispositivos são representados por P1/P2 e teclado; as telas completas estão no jogo final | quatro capturas e rota até a fase 10 |
| C++ do jogo real | 29 | struct com padrão, lambda, template, `remove_if`, `unique_ptr`, ponteiro de função, UTF-8 | trechos isolados em console, um recurso por seção | execução; diagrama capturado |
| Menu completo | 30 | cinco telas, campo de texto com `WM_CHAR`, `telaAposNomes`, mouse com escala, XInput | um nome em vez de dois; sem consulta ao ranking | `verificar_aula30`: 13 comportamentos e quatro telas exportadas |
| Joystick real | 31 | SDL2 → XInput → DirectInput, normalização, toque, foco e reconexão | hardware simulado no console | execução; roteiro manual com controle físico |
| Tela de ranking (CRUD) | 32 | listar em páginas, renomear, excluir com confirmação, um registro por chave | confirmação desenhada em vez de `MessageBoxW`; CSV próprio da aula | `verificar_aula32`: 25 comportamentos e quatro telas exportadas |
| Configuração de controles | 33 | quantos jogam, quem usa cada aparelho, `validarAtribuicao`, máscara de teclas, desconexão | joystick virtual (J/K/L) ou XInput 0; valida também quando o controle conecta | `verificar_aula33`: 23 comportamentos e cinco telas exportadas |

Cada aula possui um executável principal com janela. As Aulas 05, 18, 25, 26, 27 e 28 têm programas gráficos próprios; as demais abrem diagramas executáveis com quatro etapas, e nas Aulas 30, 32 e 33 o programa interativo fica no modo Exemplo. `ARQUITETURA_DA_RECONSTRUCAO.md` conecta os exemplos para o desafio autônomo. Os marcos jogáveis ainda são reduzidos: completar a integração de poderes, IA, ranking, chefes, áudio e controle é parte explícita da reconstrução, com o projeto final disponível como gabarito de comportamento.
