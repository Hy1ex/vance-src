#!/usr/bin/env bash

pushd "$(dirname "${BASH_SOURCE-$0}")" > /dev/null || exit
source tools-wine.sh

EXE=\"$GAME_DIR/bin/hammerplusplus.exe\"
ARGS=(-nop4)

echo Running command \"run wine "$CMD"\"
run "wine $EXE ${ARGS[*]}"

popd || exit
