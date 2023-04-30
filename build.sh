#!/usr/bin/env bash

pushd "$(dirname "${BASH_SOURCE-$0}")" > /dev/null || exit
source devtools/linux/env.sh

export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
export CLICOLOR_FORCE=1
export TERM=xterm-256color

alias gcc="gcc -fdiagnostics-color=auto"

PROJECT=games

CXXFLAGS="-w -fdiagnostics-color=always" make -f$PROJECT.mak

popd || exit
