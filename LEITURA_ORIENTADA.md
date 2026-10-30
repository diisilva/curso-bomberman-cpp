# Leitura orientada de bomberman.cpp

1. Comece por `WinMain` (fim do arquivo): inicializa DPI e GDI+, registra a classe, cria a janela e despacha mensagens.
2. Em `processarMensagem`, siga `WM_TIMER`, `WM_KEYDOWN`, `WM_KEYUP`, `WM_PAINT`, `WM_SIZE`, `WM_DPICHANGED` e `WM_DESTROY`.
3. Volte a `atualizar`: identifique fase, bombas, inimigos, chefes, bot e animação.
4. Leia `desenhar` e `garantirBuffer`: observe HDC, GDI+, caches e apresentação da imagem.
5. Siga `novaPartida` → `montarFase` → `avancarFase`; anote o que é reiniciado em cada uma.
6. Siga `colocarBombaEm` → `atualizarBombas` → `iniciarExplosaoBomba` → `propagarExplosao`.
7. Compare `celulaNoAlcance` com `propagarExplosao` usando duas caixas consecutivas.
8. Estude `lerRanking`, `gravarRanking` e `salvarRegistroPartida` com um CSV de teste.
9. Estude `atualizarEstadoControle`: SDL2, XInput e DirectInput convergem para `EntradaControle`.
10. Estude `iniciarBoss`, `atualizarBoss`, `atualizarMario` e os atlas de Hornet/Bowser.
