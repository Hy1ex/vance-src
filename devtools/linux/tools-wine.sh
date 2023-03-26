#!/usr/bin/env bash
cd "$(dirname "${BASH_SOURCE-$0}")" || exit
source env.sh

export WINEPREFIX="$SRC_DIR/.wine"
export WINEARCH="win64"

if [ ! -d "$WINEPREFIX" ]; then
	echo Creating Wine prefix...
	mkdir -p "$WINEPREFIX"
	
	wine64 wineboot --init
	
	# Configure registry
	#run wine regedit "$SRC_DIR/devtools/linux/wine-theme.reg"
	#run wine regedit "$SRC_DIR/devtools/linux/wine-breeze-dark.reg"

	# Install D3DCompiler for shadercompile
	winetricks prefix="$WINEPREFIX" d3dcompiler_47

	# Use Mono instead
	# Install .NET Framework 2.0 for VTFEdit and GCFScape
	#winetricks dotnet20sp2
	# Install .NET Framework 4.8 for Crowbar
	#winetricks dotnet48
fi

run() {
	echo "run(): $*"
	eval "$*"
}
