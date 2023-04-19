#!/usr/bin/env bash
pushd "$(dirname "${BASH_SOURCE-$0}")" > /dev/null || exit
source env.sh

export LD_LIBRARY_PATH="$GAME_DIR/bin:$LD_LIBRARY_PATH"

export WINEPREFIX="$MOD_DIR/.wine"
export WINEARCH="win64"
# Only use this when running engine-local binaries
export WINEPATH="$GAME_DIR/bin:$WINEPATH"

if [ ! -d "$WINEPREFIX" ]; then
	echo Creating Wine prefix...
	mkdir -p "$WINEPREFIX"
	
	# Configure registry
	run wine regedit "$SRC_DIR/devtools/linux/wine-theme.reg"
	#run wine regedit "$SRC_DIR/devtools/linux/wine-breeze-dark.reg"
fi

run() {
	echo "run(): $*"
	pushd "$GAME_DIR" || exit > /dev/null
	eval "$*"
	popd || exit > /dev/null
}

popd || exit
