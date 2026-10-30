param([Parameter(Mandatory=$true)][int]$Numero)
$ErrorActionPreference = "Stop"
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
$compilador = (Get-Command g++ -ErrorAction Stop).Source
if (-not (Test-Path -LiteralPath (Join-Path $raiz "projeto_final\assets"))) {
    Write-Warning "Sprites do jogo ausentes: rode .\baixar_jogo_final.ps1 (as aulas com imagens do jogo mostram 'PNG ausente')."
}
$prefixo = "Aula_{0:D2}_" -f $Numero
$pasta = Get-ChildItem -LiteralPath (Join-Path $raiz "aulas") -Directory | Where-Object { $_.Name.StartsWith($prefixo) } | Select-Object -First 1
if (-not $pasta) { throw "Pasta da aula $Numero nao encontrada." }
$arquivo = Get-Item -LiteralPath (Join-Path $pasta.FullName ($pasta.Name + ".cpp"))
if (-not $arquivo) { throw "Aula $Numero nao encontrada." }
$saida = Join-Path $raiz "build"
New-Item -ItemType Directory -Path $saida -Force | Out-Null
$nomeExe = if ($Numero -eq 1) { 'Aula_01.exe' } else { $arquivo.BaseName + '.exe' }
$executavel = Join-Path $saida $nomeExe
$visual = Join-Path $pasta.FullName ("Aula_{0:D2}_visual.cpp" -f $Numero)
$bibliotecas = @()
if ($Numero -in 4,5,18,24,25,26,27,28,30,32,33) { $bibliotecas = @('-lgdiplus','-mwindows') }
if ($Numero -eq 3) { $bibliotecas = @('-mwindows') }
if ($Numero -eq 23) { $bibliotecas = @('-lwinmm') }
if ($Numero -in 5,18,25,26,27,28) {
    # Estas aulas ja tem programa grafico proprio.
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' $arquivo.FullName '-o' $executavel @bibliotecas
    if ($LASTEXITCODE -ne 0) { throw "Falha de compilacao da aula $Numero." }
} else {
    if (-not (Test-Path -LiteralPath $visual)) { throw "Visual da aula $Numero nao encontrado." }
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' $visual '-o' $executavel '-lgdiplus' '-mwindows'
    if ($LASTEXITCODE -ne 0) { throw "Falha no visual principal da aula $Numero." }
    $pastaExemplos = Join-Path $saida 'internos\exemplos'
    New-Item -ItemType Directory -Path $pastaExemplos -Force | Out-Null
    $exemplo = Join-Path $pastaExemplos ("Aula_{0:D2}_exemplo.exe" -f $Numero)
    $fontesExemplo = @($arquivo.FullName)
    if ($Numero -notin 3,4,24,30,32,33) {
        $fontesExemplo += (Join-Path $raiz 'visual\pausa_console.cpp')
    }
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' @fontesExemplo '-o' $exemplo @bibliotecas
    if ($LASTEXITCODE -ne 0) { throw "Falha de compilacao do exemplo original da aula $Numero." }
}
Write-Host "Aula ${Numero}: $executavel"
if (Test-Path -LiteralPath $visual) {
    if ($Numero -eq 18) {
        $pastaVisuais = Join-Path $saida 'internos\visuais'
        New-Item -ItemType Directory -Path $pastaVisuais -Force | Out-Null
        & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' $visual '-o' (Join-Path $pastaVisuais 'Aula_18_visual.exe') '-lgdiplus' '-mwindows'
        if ($LASTEXITCODE -ne 0) { throw 'Falha no diagrama complementar da aula 18.' }
    }
    $pastaCapturas = Join-Path $saida 'internos\capturas'
    New-Item -ItemType Directory -Path $pastaCapturas -Force | Out-Null
    $captura = Join-Path $pastaCapturas ("Aula_{0:D2}_captura.exe" -f $Numero)
    & $compilador "-std=c++17" "-O2" "-Wall" "-Wextra" "-pedantic" "-DCAPTURA" $visual "-o" $captura "-lgdiplus"
    if ($LASTEXITCODE -ne 0) { throw "Falha no teste visual da aula $Numero." }
    Push-Location -LiteralPath $pasta.FullName
    try { & $captura; if ($LASTEXITCODE -ne 0) { throw "Captura da aula $Numero falhou." } }
    finally { Pop-Location }
    $inicio = Join-Path $pasta.FullName "imagens\etapa_1.png"
    $fim = Join-Path $pasta.FullName "imagens\etapa_4.png"
    if (-not (Test-Path $inicio) -or -not (Test-Path $fim)) { throw "Faltam imagens da aula $Numero." }
    if ((Get-Item $inicio).Length -lt 5000 -or (Get-Item $fim).Length -lt 5000) { throw "Imagem vazia da aula $Numero." }
    if ((Get-FileHash $inicio).Hash -eq (Get-FileHash $fim).Hash) { throw "Estados visuais identicos na aula $Numero." }
    Write-Host "Visual e capturas da aula ${Numero}: OK"
}
if ($Numero -in 25,26,27,28) {
    $pastaCapturas = Join-Path $saida 'internos\capturas'
    New-Item -ItemType Directory -Path $pastaCapturas -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $pasta.FullName 'imagens') -Force | Out-Null
    $captura = Join-Path $pastaCapturas ("Aula_{0:D2}_captura.exe" -f $Numero)
    & $compilador '-std=c++17' '-O2' '-Wall' '-Wextra' '-pedantic' '-DCAPTURA' $arquivo.FullName '-o' $captura '-lgdiplus'
    if ($LASTEXITCODE -ne 0) { throw "Captura da aula $Numero nao compilou." }
    Push-Location -LiteralPath $pasta.FullName
    try { & $captura; if ($LASTEXITCODE -ne 0) { throw "Captura da aula $Numero falhou." } }
    finally { Pop-Location }
    $inicio = Join-Path $pasta.FullName 'imagens\etapa_1.png'
    $fim = Join-Path $pasta.FullName 'imagens\etapa_4.png'
    if (-not (Test-Path $inicio) -or -not (Test-Path $fim)) { throw "Faltam imagens da aula $Numero." }
    if ((Get-Item $inicio).Length -lt 5000 -or (Get-Item $fim).Length -lt 5000) { throw "Imagem vazia da aula $Numero." }
    if ((Get-FileHash $inicio).Hash -eq (Get-FileHash $fim).Hash) { throw "Estados visuais identicos na aula $Numero." }
    Write-Host "Visual real e capturas da aula ${Numero}: OK"
}
