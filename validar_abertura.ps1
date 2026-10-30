$ErrorActionPreference='Stop'
$raiz=Split-Path -Parent $MyInvocation.MyCommand.Path
$pastas=Get-ChildItem -LiteralPath (Join-Path $raiz 'aulas') -Directory
$casos=@()
# Os exemplos abrem em segundo plano; o da Aula 23 nao deve tocar musica durante o teste.
$env:CURSO_BOMBERMAN_SEM_SOM='1'
foreach ($numero in 1..33) {
    $prefixo='Aula_{0:D2}_' -f $numero
    $pasta=$pastas | Where-Object { $_.Name.StartsWith($prefixo) } | Select-Object -First 1
    if (-not $pasta) { throw "Pasta da aula $numero nao encontrada." }
    $nomeExe=if ($numero -eq 1) { 'Aula_01.exe' } else { $pasta.Name+'.exe' }
    $casos+=@{ Numero=$numero; Tipo='principal visual'; Diretorio=(Join-Path $raiz 'build'); Exe=(Join-Path $raiz ('build\'+$nomeExe)) }
    if ($numero -notin 5,18,25,26,27,28) {
        $pastaExemplos=Join-Path $raiz 'build\internos\exemplos'
        $casos+=@{ Numero=$numero; Tipo='exemplo original'; Diretorio=$pastaExemplos; Exe=(Join-Path $pastaExemplos ('Aula_{0:D2}_exemplo.exe' -f $numero)) }
    }
}
foreach ($caso in $casos) {
    if (-not (Test-Path -LiteralPath $caso.Exe)) { throw "Executavel ausente: $($caso.Exe)" }
    $processo=$null
    try {
        # Simula duplo clique: cada executavel roda em processo separado, a partir de sua pasta.
        $processo=Start-Process -FilePath $caso.Exe -WorkingDirectory $caso.Diretorio -WindowStyle Hidden -PassThru
        Start-Sleep -Milliseconds 550
        $processo.Refresh()
        if ($processo.HasExited) { throw "Aula $($caso.Numero) $($caso.Tipo) fechou logo apos abrir (codigo $($processo.ExitCode))." }
        Write-Host "Janela da aula $($caso.Numero) $($caso.Tipo): iniciou"
    }
    finally {
        if ($processo -and -not $processo.HasExited) { Stop-Process -Id $processo.Id -Force }
    }
}
$nomesEsperados=@($casos | Where-Object {$_.Tipo -eq 'principal visual'} | ForEach-Object {[System.IO.Path]::GetFileName($_.Exe)})
$extras=@(Get-ChildItem -LiteralPath (Join-Path $raiz 'build') -File -Filter '*.exe' | Where-Object {$_.Name -notin $nomesEsperados})
if ($extras.Count -gt 0) { throw "Executaveis auxiliares ainda na raiz de build: $($extras.Name -join ', ')" }
Write-Host "Janelas verificadas: $($casos.Count)"
