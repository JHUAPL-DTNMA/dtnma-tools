#!/bin/bash
set -e
SELFDIR=$(readlink -f $(dirname "${BASH_SOURCE[0]}"))

cat ${SELFDIR}/dictionary.txt | sort | uniq >${SELFDIR}/dictionary-sort.txt
mv ${SELFDIR}/dictionary-sort.txt ${SELFDIR}/dictionary.txt
