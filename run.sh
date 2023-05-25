#!/bin/bash
#
set -e

SELFDIR=$(realpath $(dirname "${BASH_SOURCE[0]}"))
source setenv.sh

exec $@
