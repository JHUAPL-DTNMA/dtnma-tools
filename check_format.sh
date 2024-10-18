#!/bin/bash
./apply_format.sh
changed=$(git status -s | grep "^ M ")

if [ ! -z "${changed}" ]; then
  echo "Error: Files changed after formatting:"
  git diff
  exit 1
fi

true
