#!/bin/bash


find . \( ! -name . -prune \) -name "*.cpp" | xargs cat | wc -l
find . \( ! -name . -prune \) -name "*.h" | xargs cat | wc -l

