#!/bin/bash

emacs `find . \( -path "*tests/.test*" \) -prune -o -name "*.cc" -o -name "*.h" -o -name "*.py" -o -name "*.paf" -o -name "*.i" -o -name "*.table" -o -name "*.dox"`
