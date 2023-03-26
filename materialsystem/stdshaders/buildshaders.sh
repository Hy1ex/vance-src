#!/usr/bin/env bash
pushd "$(dirname "${BASH_SOURCE-$0}")" || exit
source ../../devtools/linux/env.sh
source "$SRC_DIR/devtools/linux/tools-wine.sh"

"$SRC_DIR/devtools/shadercompile/buildshaders.sh" stdshader_dx9_20b -20b $@
"$SRC_DIR/devtools/shadercompile/buildshaders.sh" stdshader_dx9_30 -30 $@

popd || exit
