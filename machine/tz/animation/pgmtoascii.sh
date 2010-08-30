#!/bin/sh
# Converts all pgm's in current dir to ASCII format
find . -name "*.pgm" -exec convert {} -compress none {} \;
