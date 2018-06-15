#!/bin/bash
#
# File:     demo.sh
# Descr:    Runs demo application with different command line arguments
# Author:   Anton Novoselov @ 2018
# URL:      https://github.com/ans-hub/game_console
#
# ############################################################################

# Check which version present

if [[ -e demo ]]; then
  APP=demo
else
  APP=demo_debug
fi

# Small world with middle objects

if [[ "$1" == 1 ]]; then
   ./"${APP}" ../../00_data/objects/cube_flat.ply 100 100 10 5

# Small world with lot of middle objects

elif [[ "$1" == 2 ]]; then
   ./"${APP}" ../../00_data/objects/cube_flat.ply 100 1000 10 5

# Big world with a lot of small objects

elif [[ "$1" == 3 ]]; then
   ./"${APP}" ../../00_data/objects/cube_flat.ply 1000 10000 0.1 4

# Big world with extremely count of small objects

elif [[ "$1" == 4 ]]; then
   ./"${APP}" ../../00_data/objects/cube_flat.ply 1000 50000 0.1 4

fi