$ErrorActionPreference = "Stop"
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
foreach ($numero in 1..33) {
    & (Join-Path $raiz "compilar_aula.ps1") -Numero $numero
    if ($LASTEXITCODE -ne 0) { throw "Aula $numero falhou." }
}
& (Join-Path $raiz 'organizar_build.ps1')
