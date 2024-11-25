#!/bin/bash
./apply_format.sh

changed=$(git status --porcelain=1)
if [ -n "${changed}" ]; then
  echo "Error: Files changed after formatting:"
  git diff
  exit 1
fi

exit 0
