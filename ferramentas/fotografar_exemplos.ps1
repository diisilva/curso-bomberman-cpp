# Gera imagens/exemplo.png (e solucao.png) das aulas cujo exemplo abre uma janela.
# Nenhuma janela aparece: ferramentas/foto_exemplo.cpp desenha num bitmap.
param([int[]]$Numeros = @(4, 24))
$ErrorActionPreference = 'Stop'
$raiz = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$compilador = (Get-Command g++ -ErrorAction Stop).Source
$pastaExe = Join-Path $raiz 'build\internos\fotos'
New-Item -ItemType Directory -Path $pastaExe -Force | Out-Null
foreach ($numero in $Numeros) {
    $prefixo = 'Aula_{0:D2}_' -f $numero
    $pasta = Get-ChildItem -LiteralPath (Join-Path $raiz 'aulas') -Directory | Where-Object { $_.Name.StartsWith($prefixo) } | Select-Object -First 1
    $fontes = @{
        'exemplo' = Join-Path $pasta.FullName ($pasta.Name + '.cpp')
        'solucao' = Join-Path $pasta.FullName ('solucoes\' + $pasta.Name + '_solucao.cpp')
    }
    foreach ($tipo in $fontes.Keys) {
        $fonte = $fontes[$tipo] -replace '\\', '/'
        $exe = Join-Path $pastaExe ("Aula_{0:D2}_{1}.exe" -f $numero, $tipo)
        # Um envoltório com o caminho escrito no arquivo evita problemas de aspas na linha de comando.
        $envoltorio = Join-Path $pastaExe ("Aula_{0:D2}_{1}.cpp" -f $numero, $tipo)
        $fotografo = (Join-Path $raiz 'ferramentas\foto_exemplo.cpp') -replace '\\', '/'
        [System.IO.File]::WriteAllText($envoltorio, "#define EXEMPLO `"$fonte`"`n#include `"$fotografo`"`n", (New-Object System.Text.UTF8Encoding $false))
        & $compilador '-std=c++17' '-O2' '-static' $envoltorio '-o' $exe '-lgdiplus' '-lgdi32'
        if ($LASTEXITCODE -ne 0) { throw "Foto da aula $numero ($tipo) nao compilou." }
        $saida = Join-Path $pasta.FullName "imagens\$tipo.png"
        Push-Location -LiteralPath $pasta.FullName
        try { & $exe $saida } finally { Pop-Location }
        if ($LASTEXITCODE -ne 0) { throw "Foto da aula $numero ($tipo) falhou." }
        Write-Host "Aula ${numero}: imagens\$tipo.png"
    }
}
