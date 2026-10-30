$ErrorActionPreference = "Stop"
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
$compilador = (Get-Command g++ -ErrorAction Stop).Source
$saida = Join-Path $raiz "build"
New-Item -ItemType Directory -Path $saida -Force | Out-Null
foreach ($nivel in 1..3) {
    $executavel = Join-Path $saida ("Marco_" + $nivel + ".exe")
    & $compilador "-std=c++17" "-O2" "-Wall" "-Wextra" "-pedantic" "-DNIVEL=$nivel" (Join-Path $raiz "marcos/jogo_incremental.cpp") "-o" $executavel "-lgdiplus" "-mwindows"
    if ($LASTEXITCODE -ne 0) { throw "Falha no marco $nivel" }
    Write-Host $executavel
}
