function ZipFiles( $zipfilename, $sourcedir )
{
	# remove existing zip if exists and add compression assembly
	if (Test-Path $zipfilename) { Remove-Item $zipfilename }
	Add-Type -Assembly System.IO.Compression.FileSystem

	# create temporary directory
	$tmpdir = Join-Path -Path $PSScriptRoot -ChildPath 'TempZipDir'
	if (Test-Path -PathType Container $tmpdir) { Remove-Item $tmpdir -Recurse -Force | Out-Null }
	New-Item -ItemType Directory -Force -Path $tmpdir | Out-Null

	# add exe and dll files to zip
	Get-ChildItem -Path $sourcedir | 
		Where-Object { $_.Name -Match "^.+\.(exe|dll)$" } |
		Copy-Item -Destination $tmpdir

	# zip temp dir
	$compressionLevel = [System.IO.Compression.CompressionLevel]::Optimal
	[System.IO.Compression.ZipFile]::CreateFromDirectory($tmpdir, $zipfilename, $compressionLevel, $false)

	# dispose of temp dir
	if (Test-Path -PathType Container $tmpdir) { Remove-Item $tmpdir -Recurse -Force | Out-Null }
}

# set env vars if not exist
if (-not (Test-Path env:SolutionDir)) { $env:SolutionDir = Join-Path -Path $PSScriptRoot -ChildPath '.' }
if (-not (Test-Path env:ProjectDir))  { $env:ProjectDir = Join-Path -Path $env:SolutionDir -ChildPath 'CueTest' }
if (-not (Test-Path env:OutDir))      { $env:OutDir     = Join-Path -Path $PSScriptRoot -ChildPath '\x64\Release\' }

# read env vars
$SolutionDir = $env:SolutionDir
$ProjectDir  = $env:ProjectDir
$OutDir      = $env:OutDir

# copy DLLs to out dir
Get-ChildItem -Path $ProjectDir | Where-Object { $_.Name -Match "^.+\.dll$" } | Copy-Item -Destination $OutDir

# remove previous zip file
$OutZipMoveTarget = Join-Path -Path $OutDir -ChildPath 'CueTest.zip'
if (Test-Path $OutZipMoveTarget) { Remove-Item $OutZipMoveTarget }

# zip output dir
$OutZip = Join-Path -Path $SolutionDir -ChildPath 'CueTest.zip'
ZipFiles $OutZip $OutDir

# move zip to output dir
Move-Item $OutZip $OutZipMoveTarget
