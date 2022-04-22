#!/usr/bin/env bash

MOD_SPECTATOR_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_SPECTATOR_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_SPECTATOR_ROOT"/conf/conf.sh" ]; then
    source $MOD_SPECTATOR_ROOT"/conf/conf.sh"
fi
