#!/usr/bin/env bash
cd "$(dirname "${BASH_SOURCE-$0}")" || exit
source ../linux/env.sh

# Short script to build shaders- ends up invoking process_shaders.py 

set -e 

START=$SECONDS

echo "==== buildshaders.sh" "$@" "===="

# Common vars 
shaderdir=shaders
inputbase="$1"
targetdir="../../../game/$MOD/shaders"

# Parse arguments
JOBS=$(nproc)
SHVER=30
USE_LIST=0
shift
for a in "$@"; do
	case $a in
		-20b)
			SHVER=20b
			shift;
			;;
		-30)
			SHVER=30
			shift;
			;;
		-j*)
			JOBS=${$a//-j//}
			shift; 
			;;
		-l)
			USE_LIST=1
			shift; 
			;;
	esac
done

# Check that we have an argument actually passed 
if [ -z "$inputbase" ]; then
	echo "USAGE: buildshaders.sh <shaderProject> [-l] [-30|-20b|-20] <shaders>"
	exit 1 
fi

pushd "$SRC_DIR/materialsystem/stdshaders" || exit

# Check that input base exists 
if [ ! -f "$inputbase.txt" ]; then
	echo "Error: $inputbase does not exist!"
	exit 1
fi

# Make any more relevant dirs
mkdir -p "$shaderdir"
mkdir -p "$shaderdir/fxc"
mkdir -p include

echo "Building inc files and worklist for $inputbase..."

# Run the shadercompiler
if [ $USE_LIST -ne 0 ]; then
	# Go through the list and append the .fxc part
	CMD=""
	for a in "$@"; do
		CMD="$CMD$a.fxc "
	done

	python3 "$SRC_DIR/devtools/shadercompile/process_shaders.py" -Threads "$JOBS" -Version $SHVER -List "$CMD"
else
	python3 "$SRC_DIR/devtools/shadercompile/process_shaders.py" -Threads "$JOBS" -Version $SHVER "$inputbase.txt"
fi

# Publish any generated files
mkdir -p "$targetdir"
if [[ "$targetdir" != "$shaderdir" ]]; then
	mkdir -p "$targetdir/fxc"
	# Only copy compiled shaders if we're working off an explicit list
	if [ $USE_LIST -ne 0 ]; then
		for a in "$@"; do
			cp -fv "$shaderdir/fxc/$a.vcs" "$targetdir/fxc/$a.vcs"
		done
	else
		cp -rfv "$shaderdir/fxc/"* "$targetdir/fxc/"
	fi
fi

echo -e "\nFinished in $(($SECONDS - $START)) seconds"

popd || exit
