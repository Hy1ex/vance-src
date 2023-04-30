#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE-$0}")" || exit
source devtools/linux/env.sh

EXE=\"$GAME_DIR/hl2_linux\"
ARGS=(-game "$MOD_DIR" -windowed -w 1920 -h 1080 -dev -vulkan +map d1_town_05 "$*")

echo Running command run ./hl2_linux "$EXE" "${ARGS[@]}"

run "$EXE" "${ARGS[@]}"
