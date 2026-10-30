$ErrorActionPreference = "Stop"
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
$pasta = Join-Path $raiz "projeto_final"
if (-not (Test-Path -LiteralPath (Join-Path $pasta "bomberman.cpp"))) { throw "Jogo final ausente: rode .\baixar_jogo_final.ps1" }
$compilador = (Get-Command g++ -ErrorAction Stop).Source
$testes = @("verificar_colisoes","verificar_ranking_pontuacao","verificar_controle","verificar_novos_bosses","verificar_mario_aliado","verificar_insano")
Push-Location $pasta
try {
    foreach ($nome in $testes) {
        $fonte = Join-Path "testes" ($nome + ".cpp")
        $executavel = Join-Path $pasta ($nome + "_curso.exe")
        & $compilador "-std=c++17" "-O2" "-Wall" "-Wextra" "-pedantic" $fonte "-o" $executavel "-lgdiplus" "-lwinmm" "-ldinput8" "-ldxguid" "-lgdi32" "-luser32" "-lshell32"
        if ($LASTEXITCODE -ne 0) { throw "Falha ao compilar $nome" }
        & $executavel
        if ($LASTEXITCODE -ne 0) { throw "Falha no teste $nome" }
    }
} finally {
    Pop-Location
}
