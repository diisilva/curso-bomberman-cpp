# Progresso e ponto de retomada

## Entregue

- 33 aulas em 11 trilhas, 33 executáveis principais visuais, 33 soluções, três marcos jogáveis. O jogo final (com assets e Modo Insano) fica no [GitHub](https://github.com/diisilva/bomberman-univali-prog-2); `baixar_jogo_final.ps1` o baixa para `projeto_final/`, que não é versionada no curso.
- A versão da Aula 05 editada pelo professor é a fonte de verdade. Seu programa explora três folhas reais, grade regular e irregular, cor chave, origem/destino e quatro direções. O fonte foi preservado; o roteiro, a solução e as capturas acompanham essa versão.
- 33 executáveis principais com janela. Em 27 aulas, o diagrama interativo de quatro etapas ocupa o executável principal e o exemplo original fica no modo Exemplo do menu; as Aulas 05, 18, 25, 26, 27 e 28 têm programas gráficos próprios. A Aula 27 usa as dez arenas, e a 28 usa o preview real do menu.
- Aula 18 reescrita com a mesma tabela de recortes da Aula 05: interpolação lógica/visual, pose baseada em distância, bloqueio de parede e diagnóstico de asset. Seu teste exporta repouso e movimento. Aulas 25 e 26 mostram Mario, maçã e os cinco chefes com recortes, velocidades, tempos, vida e poderes reais.
- Aula 27 mostra a montagem do mapa 13 × 15 com oito spawns e bônus, quatro etapas e uma galeria comparativa das dez arenas reais. Aula 28 mostra o fluxo do menu, as escolhas de jogadores/dificuldade/fase e o preview real do Casino Azul.
- Aula 29 reúne o C++ que o jogo usa; Aula 30 constrói um menu completo; Aula 31 conecta um joystick real (SDL2, XInput, DirectInput).
- Aula 32 (2026-10-29): a tela de ranking com o CRUD completo — páginas de dez, renomear sem perder pontos nem colidir com outro nome, excluir com confirmação, um registro por chave de nome. Programa interativo, teste com 25 comportamentos e solução "desfazer exclusão".
- Aula 33 (2026-10-29): configuração de controles — quantos jogam, quem usa teclado e joystick (P1, SEM USO, P2), `validarAtribuicao`, máscara de teclas e desconexão. Joystick virtual para estudar sem controle físico, teste com 23 comportamentos e solução "lembrar a configuração em arquivo". A aula documenta uma inconsistência do jogo real: conectar o controle com a tela aberta em 2P mostra o joystick em SEM USO até o ENTER.
- Trilhas atualizadas: Ranking e arquivos (13 → 21 → 32); Entrada e menus (16 → 31 → 33 → 28 → 30).
- `ARQUITETURA_DA_RECONSTRUCAO.md` organiza contratos, unidades e ordem de integração; `DESAFIO_DE_RECONSTRUCAO.md` fornece onze etapas verificáveis para refazer o jogo sem colar o código final.
- Scripts para compilar cada aula, todas as aulas, todas as soluções, marcos e projeto final. `validar_aulas.ps1` compila, executa e confere exemplos e capturas.

## Verificado em 2026-10-29

- 171 trechos reais conferidos com `projeto_final/bomberman.cpp`; 33 aulas e 33 soluções compilaram; 21 exemplos de console com saída esperada; testes sem janela das Aulas 05, 18, 30, 32 e 33 e os três marcos passaram. Detalhes em `VALIDACAO.md`.
- A Aula 30 voltou a compilar (faltava o diagrama gerado). `validar_abertura.ps1` não foi rodado nesta data.

## Verificado em 2026-10-13

- 28 executáveis principais, 22 diagramas e 28 soluções compilaram em C++17. O aviso de ponteiro de função da Aula 16 foi removido no exemplo e na solução; a solução da Aula 09 teve a intenção do laço explicitada.
- 19 exemplos de console executaram com saída esperada. Casos importantes: Aula 07 `destino=2 visual=1.5`; Aula 08 `#.***#`; Aula 10 primeiro passo `(2,1)`; Aula 11 `prevista=0 real=1`; Aula 20 fuga em dois passos; Aula 23 arquivo de 4044 bytes.
- 46 capturas dos diagramas foram geradas e tiveram tamanho e estados distintos verificados. Quatro capturas da Aula 05, duas da Aula 18, duas da Aula 25 e sete da Aula 26 foram exportadas; os cinco chefes foram inspecionados visualmente.
- Os 28 executáveis principais e os 22 exemplos originais iniciaram e permaneceram abertos no teste de janela oculta, com diretório de trabalho igual à pasta do executável para simular duplo clique. `ABRIR_AULAS.cmd` oferece menu; os 19 exemplos de console também aguardam Enter ao abrir diretamente em `build/internos/exemplos/`.
- Três marcos e os executáveis `bomberman.exe` e `modo_insano.exe` compilaram. Regressões do projeto final passaram para colisões, ranking/pontuação, controle simulado, Hornet/Bowser, Mario e Modo Insano.
- O nome do executável da Aula 01 passou a `build/Aula_01.exe`: o nome anterior, com acentos, era criado com caracteres inválidos pelo toolchain e não executava pelo comando do roteiro.

## Limites que o aluno deve conhecer

- As Aulas 27 e 28 isolam a montagem do mapa e o caminho principal do menu com dados e sprites do `bomberman.cpp` original. O laboratório da Aula 27 mantém a matriz ao trocar de textura para facilitar comparação; o jogo real sorteia um mapa novo ao avançar de fase. A Aula 28 representa a digitação de nomes e os dispositivos em vez de repetir suas interfaces completas; elas podem ser estudadas no projeto final.

- Os marcos jogáveis chegam a uma campanha curta; os sistemas avançados são estudados em exemplos separados e conectados pelo caderno de arquitetura e pelo desafio final. Integrá-los é trabalho do aluno, com critérios de passagem, não uma etapa automática do curso.
- As soluções mostram uma variação principal de cada aula; exercícios de aplicação e o desafio final exigem implementação própria.
- As capturas verificam o desenho GDI+ em ambiente sem janela interativa. Restam testes manuais de navegação no jogo final e nos marcos, reprodução audível, DPI em outros monitores e controle físico. A Aula 16 foi executada sem controle conectado.

## Para retomar

Na raiz do curso, rode `.\validar_aulas.ps1` para repetir build, execuções e capturas (rode pelo PowerShell). Os roteiros foram conferidos contra o commit `bf2fb5e` (2026-10-13), que `baixar_jogo_final.ps1` baixa por padrão. A versão `main` do GitHub tem as mudanças de 2026-10-19 a 29 (correção das bombas, músicas via MCI numa thread, laço do jogo no `WinMain`, poderes separados por jogador): com `-Versao main`, 6 marcadores deixam de existir e 30 roteiros mudam; acompanhá-la exige revisar ao menos as Aulas 06, 15, 17 e 23 e a trilha de poderes. Abra a Aula 05 para revisar o método visual, depois a Aula 18 para ver sua extensão. Continue por `ARQUITETURA_DA_RECONSTRUCAO.md` e pelas onze etapas de `DESAFIO_DE_RECONSTRUCAO.md`.
