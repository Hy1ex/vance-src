#!/usr/bin/env bash

pushd()
{
    command pushd "$@" > /dev/null || exit
}

popd()
{
    command popd > /dev/null || exit
}

source()
{
	command source "$@" > /dev/null
}

pushd "$(dirname "${BASH_SOURCE-$0}")" > /dev/null || exit

export MOD="vance"

export GAME_DIR="/steam/steamapps/common/Half-Life 2"
SRC_DIR="$(realpath ../..)"
export SRC_DIR
ROOT_DIR="$(realpath ../../..)"
export ROOT_DIR
MOD_DIR="$(realpath "$SRC_DIR/../game/$MOD")"
export MOD_DIR

popd || exit
