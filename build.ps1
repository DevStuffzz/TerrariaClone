<#
.SYNOPSIS
    Generates the Visual Studio solution, builds it, copies assets next to the
    executables, and optionally runs the game or the renderer tests.
.EXAMPLE
    .\build.ps1 -Run
.EXAMPLE
    .\build.ps1 -Configuration Release -Test
.EXAMPLE
    .\build.ps1 -GenerateOnly
#>
[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [switch]$Run,
    [switch]$Test,
    # Only generate TerrariaClone.sln (for working in Visual Studio).
    [switch]$GenerateOnly
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$premakeVersion = '5.0.0'

function Get-Premake {
    $onPath = Get-Command premake5 -ErrorAction SilentlyContinue
    if ($onPath) {
        return $onPath.Source
    }

    $local = Join-Path $root 'build\tools\premake5.exe'
    if (-not (Test-Path $local)) {
        Write-Host "premake5 not found on PATH; downloading Premake $premakeVersion..."
        $toolsDir = Split-Path $local
        New-Item -ItemType Directory -Force $toolsDir | Out-Null
        $zip = Join-Path $toolsDir 'premake.zip'
        $url = "https://github.com/premake/premake-core/releases/download/v$premakeVersion/premake-$premakeVersion-windows.zip"
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        Invoke-WebRequest $url -OutFile $zip -UseBasicParsing
        Expand-Archive $zip -DestinationPath $toolsDir -Force
        Remove-Item $zip
    }
    return $local
}

function Get-MSBuild {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) {
        throw 'vswhere.exe not found. Install Visual Studio 2022 with the Desktop development with C++ workload.'
    }
    $msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' |
        Select-Object -First 1
    if (-not $msbuild) {
        throw 'MSBuild not found. Install Visual Studio 2022 with the Desktop development with C++ workload.'
    }
    return $msbuild
}

function Invoke-Checked([string]$exe, [string[]]$arguments) {
    & $exe @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$(Split-Path $exe -Leaf) failed with exit code $LASTEXITCODE"
    }
}

Push-Location $root
try {
    if (-not (Test-Path 'vendor\glfw\include\GLFW\glfw3.h') -or -not (Test-Path 'vendor\stb\stb_image.h')) {
        Write-Host 'Initializing submodules...'
        Invoke-Checked 'git' @('submodule', 'update', '--init', '--recursive')
    }

    Invoke-Checked (Get-Premake) @('vs2022')
    if ($GenerateOnly) {
        Write-Host "Generated $root\TerrariaClone.sln"
        return
    }
    Invoke-Checked (Get-MSBuild) @(
        'TerrariaClone.sln',
        "/p:Configuration=$Configuration",
        '/p:Platform=x64',
        '/m',
        '/v:minimal',
        '/nologo'
    )

    $outDir = Join-Path $root "build\bin\$Configuration-windows-x86_64"
    $assets = Join-Path $root 'assets'
    if (Test-Path $assets) {
        # /MIR also removes assets that were deleted from the source folder.
        robocopy $assets (Join-Path $outDir 'assets') /MIR /NJH /NJS /NFL /NDL /NP | Out-Null
        if ($LASTEXITCODE -ge 8) {
            throw "Copying assets failed with robocopy exit code $LASTEXITCODE"
        }
        $global:LASTEXITCODE = 0
        Write-Host "Copied assets to $outDir\assets"
    }

    # Run from the output directory so relative paths like "assets/dirt.png" resolve.
    Push-Location $outDir
    try {
        if ($Test) {
            Invoke-Checked '.\RendererTests.exe' @()
        }
        if ($Run) {
            Invoke-Checked '.\TerrariaClone.exe' @()
        }
    } finally {
        Pop-Location
    }
} finally {
    Pop-Location
}
