#!/bin/sh

if [ ! -e "./canfigger.h" ]; then
  echo "Run this script from the source root directory."
  exit 1
fi

doxygen doxygen/Doxyfile
