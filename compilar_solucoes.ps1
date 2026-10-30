$ErrorActionPreference = 'Stop'
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
$compilador = (Get-Command g++ -ErrorAction Stop).Source
$saida = Join-Path $raiz 'build\internos\solucoes'
New-Item -ItemType Directory -Path $saida -Force | Out-Null
foreach ($numero in 1..33) {
    $prefixo = 'Aula_{0:D2}_' -f $numero
    $pasta = Get-ChildItem -LiteralPath (Join-Path $raiz 'aulas') -Directory | Where-Object { $_.Name.StartsWith($prefixo) } | Select-Object -First 1
    if (-not $pasta) { throw "Pasta da solucao $numero ausente." }
    $fonte = Join-Path $pasta.FullName ('solucoes\' + $pasta.Name + '_solucao.cpp')
    if (-not (Test-Path -LiteralPath $fonte)) { throw "Solucao $numero ausente." }
    $bibliotecas = @()
    if ($numero -in 4,5,18,24,25,26,27,28,30) { $bibliotecas = @('-lgdiplus','-mwindows') }
    if ($numero -eq 3) { $bibliotecas = @('-mwindows') }
    if ($numero -eq 23) { $bibliotecas = @('-lwinmm') }
    # 32 e 33: solucoes de console que incluem o programa grafico da aula.
    if ($numero -in 32,33) { $bibliotecas = @('-lgdiplus','-lgdi32') }
    $exe = Join-Path $saida ('Aula_{0:D2}_solucao.exe' -f $numero)
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' $fonte '-o' $exe @bibliotecas
    if ($LASTEXITCODE -ne 0) { throw "Solucao $numero nao compilou." }
    Write-Host "Solucao ${numero}: OK"
}
