$ErrorActionPreference='Stop'
$raiz=Split-Path -Parent $MyInvocation.MyCommand.Path
while ($true) {
    Clear-Host
    Write-Host 'CURSO BOMBERMAN C++ - ABRIR AULA' -ForegroundColor Cyan
    Write-Host 'Digite um numero de 1 a 33 ou S para sair. Ordem sugerida: veja PLANO_DO_CURSO.md'
    $texto=(Read-Host 'Aula').Trim()
    if ($texto -match '^[Ss]$') { break }
    $numero=0
    if (-not [int]::TryParse($texto,[ref]$numero) -or $numero -lt 1 -or $numero -gt 33) {
        Write-Host 'Escolha uma aula de 1 a 33.' -ForegroundColor Yellow
        Read-Host 'Enter para continuar' | Out-Null
        continue
    }
    Write-Host 'V = aula visual (padrao); E = exemplo de codigo; R = solucao.'
    $opcao=(Read-Host 'Modo').Trim().ToUpperInvariant()
    $modo=switch ($opcao) { 'E' {'Exemplo'} 'R' {'Solucao'} default {'Visual'} }
    try { & (Join-Path $raiz 'abrir_aula.ps1') -Numero $numero -Modo $modo }
    catch { Write-Host $_.Exception.Message -ForegroundColor Red }
    Read-Host 'Enter para voltar ao menu' | Out-Null
}
