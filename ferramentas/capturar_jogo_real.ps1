# Gera imagens_jogo_real/*.png a partir do código de projeto_final/bomberman.cpp.
# Nenhuma janela é aberta: o capturador desenha cada tela num bitmap em memória.
$ErrorActionPreference = 'Stop'
$raiz = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$compilador = (Get-Command g++ -ErrorAction Stop).Source
$pastaExe = Join-Path $raiz 'build\internos\jogo_real'
New-Item -ItemType Directory -Path $pastaExe -Force | Out-Null

# O jogo procura assets/ e ranking.csv ao lado do executável. Uma junção evita
# copiar 14 MB, e o ranking de demonstração não toca no ranking verdadeiro.
$assets = Join-Path $pastaExe 'assets'
if (-not (Test-Path -LiteralPath $assets)) {
    cmd /c mklink /J "$assets" "$(Join-Path $raiz 'projeto_final\assets')" | Out-Null
}
$ranking = @(
    '12/10/2026;Ana;4210;31;402'
    '12/10/2026;Bruno;3980;28;377'
    '13/10/2026;Carla;3120;40;512'
    '13/10/2026;Davi;2675;22;301'
    '14/10/2026;Eva;1840;19;260'
)
[System.IO.File]::WriteAllLines((Join-Path $pastaExe 'ranking.csv'), $ranking)

$exe = Join-Path $pastaExe 'capturar_jogo_real.exe'
& $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-static' (Join-Path $raiz 'ferramentas\capturar_jogo_real.cpp') '-o' $exe `
    '-lgdiplus' '-lwinmm' '-ldinput8' '-ldxguid' '-lgdi32'
if ($LASTEXITCODE -ne 0) { throw 'O capturador do jogo real nao compilou.' }

$saida = Join-Path $raiz 'imagens_jogo_real'
& $exe $saida
if ($LASTEXITCODE -ne 0) { throw "O capturador terminou com codigo $LASTEXITCODE." }
Write-Host "Capturas do jogo real em $saida"
