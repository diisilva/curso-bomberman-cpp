param(
    [Parameter(Mandatory=$true)][ValidateRange(1,33)][int]$Numero,
    [ValidateSet('Visual','Exemplo','Solucao')][string]$Modo='Visual'
)
$ErrorActionPreference='Stop'
$raiz=Split-Path -Parent $MyInvocation.MyCommand.Path
$prefixo='Aula_{0:D2}_' -f $Numero
$pasta=Get-ChildItem -LiteralPath (Join-Path $raiz 'aulas') -Directory | Where-Object { $_.Name.StartsWith($prefixo) } | Select-Object -First 1
if (-not $pasta) { throw "Aula $Numero nao encontrada." }

$nomeExemplo=if ($Numero -eq 1) { 'Aula_01.exe' } else { $pasta.Name+'.exe' }
$arquivo=if ($Modo -eq 'Solucao') {
    Join-Path $raiz ('build\internos\solucoes\Aula_{0:D2}_solucao.exe' -f $Numero)
} elseif ($Modo -eq 'Exemplo' -and $Numero -notin 5,18,25,26,27,28) {
    Join-Path $raiz ('build\internos\exemplos\Aula_{0:D2}_exemplo.exe' -f $Numero)
} else {
    Join-Path $raiz ('build\'+$nomeExemplo)
}
if (-not (Test-Path -LiteralPath $arquivo)) {
    if ($Modo -eq 'Solucao') { & (Join-Path $raiz 'compilar_solucoes.ps1') }
    else { & (Join-Path $raiz 'compilar_aula.ps1') -Numero $Numero }
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $arquivo)) { throw "Nao foi possivel criar $arquivo" }
}

Write-Host "Aula $Numero - $Modo" -ForegroundColor Cyan
Write-Host "Executando: $arquivo"
Push-Location -LiteralPath $pasta.FullName
$pausaAnterior = [Environment]::GetEnvironmentVariable('CURSO_BOMBERMAN_SEM_PAUSA', 'Process')
try {
    if ($Modo -eq 'Exemplo') {
        [Environment]::SetEnvironmentVariable('CURSO_BOMBERMAN_SEM_PAUSA', '1', 'Process')
    }
    & $arquivo
    $codigo=$LASTEXITCODE
} finally {
    [Environment]::SetEnvironmentVariable('CURSO_BOMBERMAN_SEM_PAUSA', $pausaAnterior, 'Process')
    Pop-Location
}
if ($codigo -ne 0) { throw "Aula $Numero terminou com codigo $codigo. Confira o roteiro da aula e os assets." }
