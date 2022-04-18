# Copyright (c) 2016, Indy Ray. All rights reserved.
# See the LICENSE file for usage, modification, and distribution terms.
Param(
    $PluginDir,
    [ValidateSet("2017", "2019", "2022")]
    $VSVersion = "2017",
    [ValidateSet("Shipping", "Development", "Test", "Debug", "DebugGame")]
    $Configuration="Development",
    $PlatformToolset,
    $WindowsSDKVer
)
$ErrorActionPreference = "Stop"

$Arch = "x64"
$HostArch = "x64"

$VSWhereUrl = "https://github.com/microsoft/vswhere/releases/download/2.8.4/vswhere.exe"

#TODO: A CMake build might be a bit cleaner, since it doesn't build in-place, but also means we need to install CMake.

if([string]::IsNullOrEmpty($PluginDir)) {
    $PluginDir = Join-Path $PSScriptRoot "..\.."
}

if([string]::IsNullOrEmpty($PlatformToolset)) {
    # Is there a better way to discover this?
    switch($VSVersion) {
        "2017" {
            $PlatformToolset="v141"
        }
        "2019" {
            $PlatformToolset="v142"
        }
        "2022" {
            $PlatformToolset="v143"
        }
        Default {
            Write-Error "Unexpected Visual Studio Version: $VSVersion."
        }
    }
}

if([string]::IsNullOrEmpty($WindowsSDKVer)) {
    $WindowsSDKVer = ((Get-ChildItem "HKLM:\Software\WOW6432Node\Microsoft\Microsoft SDKs\Windows" -ErrorAction SilentlyContinue) | % { $_.GetValue("ProductVersion") } | Sort-Object) | Select-Object -First 1
    if([string]::IsNullOrEmpty($WindowsSDKVer)) {
        $WindowsSDKVer = ((Get-ChildItem "HKLM:\Software\Microsoft\Microsoft SDKs\Windows" -ErrorAction SilentlyContinue) | % { $_.GetValue("ProductVersion") } | Sort-Object) | Select-Object -First 1
    }
    if([string]::IsNullOrEmpty($WindowsSDKVer)) {
        Write-Error "Unable to discover Windows SDK version. Ensure it is installed. You may manually specify with ex. -WindowsSDKVer=10.0.19041.0"
    }
    # Registry key seems to be missing this last sub version, how do we discover it?
    # There seems to be a SDKManifest.xml inside the directory, can that be expected in older versions?
    $WindowsSDKVer = "$WindowsSDKVer.0"
    Write-Output "Building with Windows SDK version $WindowsSDKVer"
}

switch($Configuration) {
    "Debug" {
        $BuildConfiguration = "Debug"
    }
    Default {
        $BuildConfiguration = "Release"
    }
}

# FIXME: Game always loads release, so always build release for now.
$BuildConfiguration = "Release"

$SDLDir = $env:SDL_LIBRARY
if([string]::IsNullOrEmpty($SDLDir)) {
    $SDLDir = Join-Path $PluginDir "Source\ThirdParty\SDL"
}

$Intermediate = Join-Path $PluginDir "Intermediate"
$BinDir = Join-Path $Intermediate "buildbin"

if(!(Test-Path $BinDir)) {
    New-Item -path $BinDir -type Directory
}

$VSWhere = $env:UE4SDLCONTROLLER_VSWHERE
if([string]::IsNullOrEmpty($VSWhere)) {
    $VSWhere = Join-Path $BinDir "vswhere.exe"

    if(!(Test-Path $VSWhere)) {
        Write-Output "Downloading VSWhere to $VSWhere"
        Invoke-WebRequest -Uri $VSWhereUrl -OutFile $VSWhere

        if(!(Test-Path $VSWhere)) {
            Write-Error "Failure downloading VSWhere"
        }
    }
}

$VSWhereArgs = ("-prerelease", "-latest", "-property", "installationPath")
# We append this to specifically find visual studio build tools on CI for instance.
$VSWhereArgs += ("-products", "*")

if(![string]::IsNullOrEmpty($VSVersion)){
    switch($VSVersion) {
        "2017" {
            $Range = "(15.0, 16.0]"
        }
        "2019" {
            $Range = "(16.0, 17.0]"
        }
        "2022" {
            $Range = "(17.0, 18.0]"
        }
        Default {
            Write-Error "Unexpected Visual Studio Version: $VSVersion."
        }
    }
    $VSWhereArgs += ("-version", $Range)
}

$VSInstallation = & $VSWhere @($VSWhereArgs)

if($VSInstallation -and (Test-Path "$VSInstallation\Common7\Tools\vsdevcmd.bat")) {
    & "${env:COMSPEC}" /s /c "`"$VSInstallation\Common7\Tools\vsdevcmd.bat`" -arch=$Arch -host_arch=$HostArch -no_logo && set" | foreach-object {
        $Name, $Value = $_ -split '=', 2
        # This appears to stick around for the whole process scope, can we constrain it to just child environments?
        Set-Content env:\"$Name" $Value
    }
}
else {
    Write-Error "Cannot find vsdevcmd.bat, is Visual Studio installed? Expecting $VSInstallationPath\Common7\Tools\vsdevcmd.bat"
}


$OutputBinDir = (Join-Path $PluginDir "Binaries\Win64")

Write-Output "Building SDL in $SDLDir"
Push-Location
cd $SDLDir
msbuild "$SDLDir\VisualC\SDL\SDL.vcxproj" -p:Configuration=$BuildConfiguration -p:Platform=$Arch -p:PlatformToolset=$PlatformToolset -p:WindowsTargetPlatformVersion=$WindowsSDKVer -p:OutDir=$OutputBinDir

# Where should the dll end up so that it packages correctlly, and so the game/editor can find it?
#New-Item -path (Join-Path $PluginDir "Binaries\Win64") -type Directory -ErrorAction SilentlyContinue
#cp $SDLDir\VisualC\SDL\$Arch\$BuildConfiguration\SDL2.dll (Join-Path $PluginDir "Binaries\Win64\SDL2.dll")
Pop-Location