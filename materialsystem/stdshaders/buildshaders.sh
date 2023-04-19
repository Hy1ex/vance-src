#!/usr/bin/env bash
pushd "$(dirname "${BASH_SOURCE-$0}")" > /dev/null || exit
source ../../devtools/linux/env.sh
source "$SRC_DIR/devtools/linux/tools-wine.sh"

"$SRC_DIR/devtools/shadercompile/buildshaders.py" "$(pwd)/game_shader_dx9_20b.txt" -20b $@
"$SRC_DIR/devtools/shadercompile/buildshaders.py" "$(pwd)/game_shader_dx9_30.txt" -30 $@
"$SRC_DIR/devtools/shadercompile/buildshaders.py" "$(pwd)/vance_dx9_20b.txt" -20b $@
"$SRC_DIR/devtools/shadercompile/buildshaders.py" "$(pwd)/vance_dx9_30.txt" -30 $@

popd || exit
