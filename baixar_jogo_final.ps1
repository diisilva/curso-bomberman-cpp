# Baixa o jogo final do GitHub para projeto_final/ (a pasta não faz parte do curso).
# As aulas, os marcos e as ferramentas procuram assets/ e bomberman.cpp ali.
#
# Por padrão baixa o commit contra o qual os roteiros foram conferidos (os
# trechos reais e os números de linha batem com ele). -Versao main baixa a
# versão mais recente; depois rode python ferramentas/trechos_reais.py --conferir
# para ver o que mudou.
param([string]$Versao = 'bf2fb5ec09d21b05de8ba29b2c91143e0f22b148')
$ErrorActionPreference = 'Stop'
$repositorio = 'https://github.com/diisilva/bomberman-univali-prog-2'
$raiz = Split-Path -Parent $MyInvocation.MyCommand.Path
$destino = Join-Path $raiz 'projeto_final'

if (-not (Get-Command git -ErrorAction SilentlyContinue)) { throw 'git não encontrado no PATH.' }

if (-not (Test-Path -LiteralPath (Join-Path $destino '.git'))) {
    if (Test-Path -LiteralPath $destino) {
        throw "projeto_final existe mas não é um clone de $repositorio. Remova a pasta e rode de novo."
    }
    git init --quiet $destino
    git -C $destino remote add origin $repositorio
}
# Só a versão pedida, sem o histórico inteiro.
git -C $destino fetch --depth 1 origin $Versao
if ($LASTEXITCODE -ne 0) { throw "Falha ao baixar $Versao de $repositorio." }
git -C $destino checkout --quiet --force FETCH_HEAD
if ($LASTEXITCODE -ne 0) { throw 'Falha ao preparar projeto_final.' }
Write-Host "Jogo final em $destino ($(git -C $destino log -1 --format='%h %cs %s'))"
