$ErrorActionPreference='Stop'
$raiz=Split-Path -Parent $MyInvocation.MyCommand.Path
$build=Join-Path $raiz 'build'
New-Item -ItemType Directory -Path $build -Force | Out-Null
$buildResolvido=(Resolve-Path -LiteralPath $build).Path.TrimEnd('\')
$legado=Join-Path $buildResolvido 'internos\legado'
New-Item -ItemType Directory -Path $legado -Force | Out-Null
$movidos=0
foreach ($arquivo in (Get-ChildItem -LiteralPath $buildResolvido -File -Filter '*.exe')) {
    # Apenas arquivos da raiz de build com nomes gerados pelos scripts antigos.
    $pastaArquivo=[System.IO.Path]::GetDirectoryName($arquivo.FullName).TrimEnd('\')
    if ($pastaArquivo -ne $buildResolvido) { continue }
    $nome=$arquivo.Name
    $auxiliar=($nome -match '^Aula_\d{2}_(captura|solucao|visual)\.exe$' -or
               $nome -match '^Aula_\d{2}_.+_solucao\.exe$' -or
               $nome -match '^verificar_(visual_)?aula\d{2}\.exe$' -or
               ($nome.StartsWith('Aula_01_tipos_') -and $nome.Contains([char]0xFFFD)) -or
               $nome -eq 'aula03.exe')
    if (-not $auxiliar) { continue }
    $destino=Join-Path $legado $nome
    $indice=1
    while (Test-Path -LiteralPath $destino) {
        $destino=Join-Path $legado ($arquivo.BaseName+'_'+$indice+'.exe')
        $indice++
    }
    Move-Item -LiteralPath $arquivo.FullName -Destination $destino
    $movidos++
}
$aviso=@'
PASTA DE BUILD DO CURSO

Para abrir uma aula: use ABRIR_AULAS.cmd na raiz do curso.
Todos os 33 executaveis Aula_NN da raiz abrem janelas visuais explicativas.
Use o modo E do menu para rodar o exemplo original com a saida de console legivel.
Nas Aulas 30, 32 e 33 o modo E abre o programa interativo (menu, ranking e controles).
Os .exe em internos/exemplos tambem podem ser abertos por duplo clique.
Os exemplos de console aguardam Enter; os exemplos graficos ficam abertos ate fechar.
Capturas, solucoes e testes sao auxiliares.
internos/legado guarda binarios antigos separados; pode ser removido depois de revisar.
'@
Set-Content -LiteralPath (Join-Path $buildResolvido 'LEIA-ME.txt') -Value $aviso -Encoding UTF8
Write-Host "Binarios auxiliares antigos organizados: $movidos"
