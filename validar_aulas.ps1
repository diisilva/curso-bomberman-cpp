param([switch]$SemCompilar)
$ErrorActionPreference = 'Stop'
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
$compilador = (Get-Command g++ -ErrorAction Stop).Source
if (-not (Test-Path -LiteralPath (Join-Path $raiz 'projeto_final\bomberman.cpp'))) { throw 'Jogo final ausente: rode .\baixar_jogo_final.ps1' }

# 1. Os trechos reais dos roteiros precisam bater com projeto_final/bomberman.cpp.
& python (Join-Path $raiz 'ferramentas\trechos_reais.py') --conferir
if ($LASTEXITCODE -ne 0) { throw 'Trechos reais desatualizados: rode python ferramentas/trechos_reais.py' }

if (-not $SemCompilar) {
    & (Join-Path $raiz 'compilar_todas.ps1')
    if ($LASTEXITCODE -ne 0) { throw 'Falha ao compilar aulas e visuais.' }
    & (Join-Path $raiz 'compilar_solucoes.ps1')
    if ($LASTEXITCODE -ne 0) { throw 'Falha ao compilar solucoes.' }
}

# 2. Cada exemplo de console precisa imprimir a linha que o roteiro promete.
$esperado = @{
    1='jogador=(1,2)'; 2='borda intacta=0'; 6='total=21 resto=4'; 7='direita: reserva (1,3)'
    8='caixas destruidas=2'; 9='nova fase: alcance=1 limite=1 vidas=4'; 10='primeiro passo do inimigo=(2,1)'
    11='(1,4) . prevista=0 real=1'; 12='fase=2 vidas=3 pontos=680'; 13='2. Bia 900 pts, 3 bombas, 35 passos'
    14='com subpassos de 25 ms: acertou em c=3.10'; 15='total=14992 amostras = 14992 bytes'
    16='zona morta    -> dl= 0 dc= 0'; 17='roteiro da leitura guiada'
    19='chute com inimigo em (1,5): bomba para em (1,4)'; 20='alcance 2          prazo=7 passos  alvo=1  fuga=nao'
    21='linhas ignoradas=4'; 22='H_RECUP_DANO  -> H_VOO_AGULHA'; 23='arquivo=15036 bytes'
    29='liberou jogador.png'; 31='conectou segurando A: toque=0'
}
$pausaAnterior = [Environment]::GetEnvironmentVariable('CURSO_BOMBERMAN_SEM_PAUSA', 'Process')
$somAnterior = [Environment]::GetEnvironmentVariable('CURSO_BOMBERMAN_SEM_SOM', 'Process')
[Environment]::SetEnvironmentVariable('CURSO_BOMBERMAN_SEM_PAUSA', '1', 'Process')
[Environment]::SetEnvironmentVariable('CURSO_BOMBERMAN_SEM_SOM', '1', 'Process')   # a Aula 23 nao toca musica aqui
try {
foreach ($numero in ($esperado.Keys | Sort-Object)) {
    $prefixo = 'Aula_{0:D2}_' -f $numero
    $pasta = Get-ChildItem -LiteralPath (Join-Path $raiz 'aulas') -Directory | Where-Object { $_.Name.StartsWith($prefixo) } | Select-Object -First 1
    $exe = Join-Path $raiz ('build\internos\exemplos\Aula_{0:D2}_exemplo.exe' -f $numero)
    if (-not (Test-Path -LiteralPath $exe)) { throw "Executavel da aula $numero ausente: $exe" }
    Push-Location -LiteralPath $pasta.FullName
    try { $saida = (& $exe 2>&1 | Out-String); $codigo = $LASTEXITCODE }
    finally { Pop-Location }
    if ($codigo -ne 0) { throw "Aula $numero terminou com codigo $codigo.`n$saida" }
    if (-not $saida.Contains($esperado[$numero])) { throw "Resultado inesperado na aula ${numero} (esperava '$($esperado[$numero])'):`n$saida" }
    Write-Host "Execucao da aula ${numero}: OK"
}
} finally {
    [Environment]::SetEnvironmentVariable('CURSO_BOMBERMAN_SEM_PAUSA', $pausaAnterior, 'Process')
    [Environment]::SetEnvironmentVariable('CURSO_BOMBERMAN_SEM_SOM', $somAnterior, 'Process')
}

# 3. Testes de desenho e de estado que nao abrem janela.
$testes = @(
    @{ Nome='verificar_aula05'; Fonte='testes\verificar_visual_aula05.cpp' },
    @{ Nome='verificar_aula18'; Fonte='testes\verificar_aula18.cpp' },
    @{ Nome='verificar_aula30'; Fonte='testes\verificar_aula30.cpp' },
    @{ Nome='verificar_aula32'; Fonte='testes\verificar_aula32.cpp' },
    @{ Nome='verificar_aula33'; Fonte='testes\verificar_aula33.cpp' }
)
$pastaTestes = Join-Path $raiz 'build\internos\testes'
New-Item -ItemType Directory -Path $pastaTestes -Force | Out-Null
foreach ($teste in $testes) {
    $exe = Join-Path $pastaTestes ($teste.Nome + '.exe')
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' (Join-Path $raiz $teste.Fonte) '-o' $exe '-lgdiplus' '-lgdi32'
    if ($LASTEXITCODE -ne 0) { throw "Teste $($teste.Nome) nao compilou." }
    Push-Location -LiteralPath $raiz
    try { & $exe; $codigo=$LASTEXITCODE }
    finally { Pop-Location }
    if ($codigo -ne 0) { throw "Teste $($teste.Nome) falhou com codigo $codigo." }
    Write-Host "$($teste.Nome): OK"
}

# 4. Os tres marcos: o mesmo teste compilado com -DNIVEL=1, 2 e 3.
foreach ($nivel in 1..3) {
    $exe = Join-Path $pastaTestes ("verificar_marco_$nivel.exe")
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' "-DNIVEL=$nivel" (Join-Path $raiz 'testes\verificar_marcos.cpp') '-o' $exe '-lgdiplus' '-lgdi32'
    if ($LASTEXITCODE -ne 0) { throw "Teste do marco $nivel nao compilou." }
    Push-Location -LiteralPath $raiz
    try { & $exe; $codigo=$LASTEXITCODE }
    finally { Pop-Location }
    if ($codigo -ne 0) { throw "Teste do marco $nivel falhou." }
}

# 5. Todos os executaveis principais e exemplos abrem sem fechar sozinhos.
& (Join-Path $raiz 'validar_abertura.ps1')
Write-Host 'Aulas, solucoes, execucoes, trechos e capturas: OK'
