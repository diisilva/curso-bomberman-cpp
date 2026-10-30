# Como o projeto anima sprites em C++

Este documento explica as bibliotecas e as técnicas usadas na arena de batalha.
O código principal está em `batalha.cpp`. A versão atual do executável é
`batalha_otimizada.exe`.

## Bibliotecas utilizadas

| Biblioteca ou API | Responsabilidade no projeto |
| --- | --- |
| **GDI+** (`gdiplus.h`, biblioteca `gdiplus`) | Carregar PNGs, recortar sprites, desenhar imagens, aplicar transparência, ampliar pixels e desenhar textos e efeitos. |
| **Win32 / User32** (`windows.h`, biblioteca `user32`) | Criar a janela, receber eventos do teclado, usar o temporizador e processar mensagens da aplicação. |
| **GDI** (biblioteca `gdi32`) | Criar o buffer de desenho da janela e apresentar o quadro pronto com `BitBlt`, evitando a limpeza visível entre quadros. |
| **Shell32** (`shellapi.h`, biblioteca `shell32`) | Separar os argumentos da linha de comando, como os usados para exportar capturas e executar benchmarks. |
| **Biblioteca padrão do C++** | Organizar personagens, imagens e efeitos, medir o tempo, executar cálculos e gerenciar memória. |

Não usamos uma engine de jogos, SDL, SFML ou OpenGL. A parte gráfica usa GDI+
com apoio de GDI e Win32. O compilador é o GCC/MinGW, e o código usa C++17.

O GDI+ fornece as operações de desenho. As regras de movimento, a seleção de
quadros, as colisões e o combate foram implementados no código C++ do projeto.

## 1. Animação por quadros com sprite sheets

Uma **sprite sheet** é uma imagem que reúne várias poses de um personagem.
O PNG não se anima sozinho: o programa escolhe uma pose de cada vez e troca
a imagem exibida ao longo do movimento.

Cada quadro é identificado por um retângulo:

```cpp
Rect(x, y, largura, altura);
```

Os valores indicam a posição e o tamanho do recorte dentro da folha. As funções
`prepareCharacters()` e `prepareGuests()` preparam os recortes usados na arena.

O programa mantém imagens individuais em memória para os quadros dos personagens.
Isso também evita que pixels de uma célula vizinha da folha apareçam no desenho.

## 2. Organização por personagem, direção e quadro

Os quatro Bombermans usam uma estrutura equivalente a:

```cpp
quadros[personagem][direcao][quadro]
```

No código, essa organização usa `std::array` e `std::unique_ptr<Bitmap>`.
São quatro personagens, quatro direções e três quadros por direção, totalizando
48 recortes. Mario tem 12 quadros organizados por direção e etapa da caminhada.

A ordem das direções da lógica do jogo é:

| Índice | Direção | Alteração na posição |
| --- | --- | --- |
| 0 | Baixo | Aumenta Y |
| 1 | Esquerda | Diminui X |
| 2 | Cima | Diminui Y |
| 3 | Direita | Aumenta X |

A folha original dos Bombermans tem outra ordem de linhas: baixo, direita,
cima e esquerda. Por isso, fazemos um mapeamento explícito:

```cpp
constexpr int atlasRow[] = {0, 3, 2, 1};
```

Mario tem outra distribuição de quadros, então recebe seu próprio mapeamento.
Essa separação corrige o problema de um personagem andar para um lado usando
a pose do lado oposto. As sentinelas também têm quadros separados por direção.

## 3. Troca dos quadros conforme a distância percorrida

A caminhada utiliza esta sequência:

```cpp
constexpr int cycle[] = {0, 1, 0, 2};
```

A função `walkFrame()` escolhe a etapa usando a distância acumulada pelo personagem:

```cpp
return a.walking
    ? cycle[(int)(a.walkDistance * 2.5f) % 4]
    : 1;
```

O operador `%` permite repetir o ciclo. A distância percorrida faz a animação
acompanhar o deslocamento. Quando o personagem para, fica no quadro 1.

Há duas operações diferentes acontecendo juntas:

- **Movimentação:** altera a posição em que o personagem é desenhado.
- **Animação:** altera a pose desenhada naquela posição.

Trocar imagens sem alterar a posição produz uma caminhada no lugar. Alterar
a posição sem trocar imagens faz o personagem deslizar. Combinamos as duas.

## 4. Movimento gradual e controle pelo teclado

O personagem guarda coordenadas da casa de destino (`x`, `y`) e coordenadas
decimais da posição visual (`px`, `py`). O deslocamento avança gradualmente
até o centro da casa de destino.

A relação utilizada é:

```text
distância = velocidade × tempo
```

`startMove()` verifica se a próxima casa está livre. `advanceActor()` avança
a posição visual em direção ao destino. Isso evita teletransportes entre casas.

O movimento manual é tratado por `moveManually()`, sem esperar o intervalo
de decisão da inteligência artificial. A última direção pressionada tem
prioridade, e a direção oposta permite inverter o movimento durante um passo.
As curvas acontecem nos centros das casas, respeitando os corredores.

Ao soltar as teclas, o personagem termina o passo atual e para no centro da casa.
Ao perder o foco da janela, o programa limpa o estado das teclas para evitar
movimento preso.

## 5. Tempo real e simulação em passos fixos

Usamos `std::chrono::steady_clock` para medir o tempo transcorrido entre as
atualizações da janela. A função `advanceRealTime()` acumula esse tempo e executa
a simulação em passos fixos de **1/120 de segundo**.

Isso significa que a lógica pode executar vários passos antes de desenhar o
próximo quadro. A velocidade não depende diretamente de a janela desenhar
15, 30, 60 ou 120 quadros por segundo.

**Passos de simulação não são a mesma coisa que FPS:** 120 atualizações de lógica
por segundo não garantem que o computador apresente 120 imagens por segundo.

Ao controlar Mario, o tempo consumido pela simulação é multiplicado por **0,65**.
Assim, a partida inteira fica 35% mais lenta: movimento, inimigos, bombas e
projéteis acompanham esse ritmo. No automático, o multiplicador é **0,80**, deixando
a partida 20% mais lenta. Os dois percentuais são relativos ao ritmo original,
sem acumular uma redução sobre a outra. Ao controlar White, o multiplicador
permanece em **1,0**. A câmera lenta acionada por L é um ajuste adicional.

## 6. Transparência pelo canal alfa

Algumas folhas têm um fundo colorido em volta dos personagens. Durante a
preparação dos quadros, identificamos as cores de fundo previstas para aquela
folha e transformamos esses pixels em transparentes.

```cpp
Color(0, 0, 0, 0);
```

Nesse construtor de quatro argumentos, a ordem é **alfa, vermelho, verde e azul**.
Alfa zero significa totalmente transparente. Assim, o piso da arena aparece
atrás do personagem, sem um retângulo colorido acompanhando o sprite.

Esse tratamento depende das cores e dos recortes das folhas fornecidas; não é
uma ferramenta genérica para remover qualquer fundo de qualquer imagem.

## 7. Duplo buffer para evitar piscadas

O projeto monta o quadro completo em um buffer fora da tela: cenário,
personagens, bombas, explosões e interface. Depois apresenta tudo de uma vez
na janela usando `BitBlt`.

```text
Atualizar a simulação
        ↓
Desenhar o quadro completo fora da tela
        ↓
Copiar o quadro pronto para a janela
```

Isso evita mostrar etapas intermediárias, como a janela vazia antes de os
personagens serem desenhados. O buffer é reaproveitado e recriado quando o
tamanho da janela muda.

## 8. Cache e otimização dos cenários

A folha Dungeon tem **2054 × 2903 pixels**. Apesar de o PNG ocupar aproximadamente
1,8 MB em disco, uma representação de quatro bytes por pixel corresponde a
aproximadamente **23,9 MB** de dados de pixels, sem contar outros custos.

O problema identificado estava no desenho repetido de pequenos recortes dessa
folha grande durante a reconstrução do terreno. Reduzir somente o tamanho
comprimido do arquivo não resolveria necessariamente esse custo.

Criamos `assets_otimizados/arenas.png`, com **368 × 138 pixels**, aproximadamente
20 KB em disco e 24 casas já preparadas: piso, parede e caixa para cada um dos
oito temas. Os recortes mantêm a aparência em pixel art e já incluem escala,
cor, sombra e composição do fundo.

Durante a partida:

- As texturas pequenas ficam prontas em memória.
- O terreno completo fica em um buffer reaproveitado.
- Quando uma caixa é destruída, somente a casa alterada é redesenhada.
- Quando o tema muda, o terreno é preenchido usando as casas pequenas daquele tema.

Se o atlas compacto estiver disponível ao lado do executável, o jogo não carrega
as folhas grandes de terreno. Se estiver ausente, prepara os recortes em memória
a partir dos originais e libera essas folhas depois. Os arquivos originais
não foram modificados.

No benchmark local, o custo médio de desenhar um quadro com alteração de terreno
nos três últimos temas caiu de aproximadamente **3,1–4,5 segundos** para
aproximadamente **10 milissegundos**. Esses valores são medições desse teste;
não representam uma garantia de FPS em qualquer computador.

## 9. Estruturas de dados e recursos de C++

| Recurso | Uso no projeto |
| --- | --- |
| `struct` | Representar personagens, bombas, projéteis, partículas e outros estados do jogo. |
| `std::array` | Guardar conjuntos de tamanho fixo, como personagens e quadros de animação. |
| `std::vector` | Guardar conjuntos que crescem e diminuem, como bombas, maçãs e partículas. |
| `std::deque` | Guardar os eventos recentes e a fila usada na busca de caminhos. |
| `std::unique_ptr` | Gerenciar a propriedade das imagens e liberar sua memória automaticamente. |
| `std::chrono` | Medir tempo real para a simulação. |
| `<algorithm>` | Ordenar personagens e remover efeitos encerrados. |
| `<cmath>` | Calcular distâncias, trajetórias e efeitos visuais. |
| `<random>` | Gerar escolhas da IA, distribuição de caixas e partículas. |

O uso de `std::unique_ptr` aplica o princípio de gerenciamento de recursos por
tempo de vida, conhecido como **RAII**. Os recursos de GDI+, porém, precisam ser
liberados antes de encerrar o GDI+; o programa faz essa limpeza explicitamente
no encerramento.

## 10. Colisões e interações são lógica do jogo

O GDI+ não decide se um personagem pode atravessar uma parede ou receber dano.
Essas regras são implementadas separadamente do desenho.

O terreno é uma matriz em que cada casa representa piso, parede ou caixa.
A função `walkable()` verifica a casa, as bombas e a área ocupada pelo chefe
antes de permitir um deslocamento.

As bombas geram explosões em cruz. Paredes bloqueiam o fogo; caixas recebem
a explosão e são destruídas; outras bombas podem ser detonadas em cadeia.
O chefe bloqueia a passagem, recebe dano quando o fogo alcança sua área e
deixa de bloquear após ser derrotado.

O chefe tenta se deslocar na horizontal ou vertical após quatro segundos de
descanso. A área ocupada acompanha sua posição interpolada, e o caminho é
reservado durante o deslocamento para evitar que personagens o atravessem.
Ele derruba obstáculos internos, preservando as paredes externas.

As maçãs de Mario usam um arco visual calculado com seno e verificam o impacto
ao chegar ao destino. É uma trajetória programada para o jogo, sem uma biblioteca
externa de física.

O combate usa os estados `Playing`, `Victory` e `Defeat`. As maçãs eliminam
inimigos comuns em um acerto; os derrotados permanecem fora da partida. O chefe
recebe dano na sua barra de vida. Mario suporta dois acertos e morre no terceiro,
com um intervalo de invulnerabilidade para evitar vários danos no mesmo contato.
Na derrota, a simulação para e os botões recebem cliques pela API Win32: Sim
chama `reset()` para restaurar toda a partida; Não fecha a janela.

## Como apresentar a ideia ao professor

> Implementei animação por quadros com sprite sheets. O C++ controla posição,
> direção e sequência das poses; o GDI+ desenha os recortes. Usei transparência
> alfa, atualização baseada em tempo, duplo buffer e cache para manter a
> animação fluida. As colisões e as interações foram programadas separadamente
> da renderização.

A principal ideia é combinar dados de animação, estado do personagem e tempo
de simulação. As bibliotecas fornecem janela, entrada e desenho; o comportamento
da cena vem dos algoritmos implementados no projeto.

## Arquivos para consultar

- `batalha.cpp`: implementação da arena, animação, controles e renderização.
- `testes_batalha.cpp`: testes de combate, controles, velocidade e colisões.
- `compilar_batalha.ps1`: compilação da versão atual.
- `README.md`: instruções para executar e controlar o jogo.
- `assets_otimizados/arenas.png`: atlas compacto dos oito temas.
- `desempenho_antes.csv` e `desempenho_depois.csv`: medições da renderização.

Para abrir a versão atual:

```powershell
.\batalha_otimizada.exe
```
