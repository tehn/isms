#!/bin/bash

# format code
#clang-format -i niil.c

# remove old
rm ./niil

# debug(slow)
# cc -std=c89 -DDEBUG -Wall -Wno-unknown-pragmas -Wpedantic -Wshadow -Wextra -Werror=implicit-int -Werror=incompatible-pointer-types -Werror=int-conversion -Wvla -g -Og -fsanitize=address -fsanitize=undefined nasu.c -L/usr/local/lib -lSDL2 -o nasu

# build(fast)
cc niil.c -std=c11 -Os -DNDEBUG -g0 -s -Wall -Wno-unknown-pragmas -L/usr/local/lib -lSDL2 -o niil

# Size
echo "Size: $(du -sk ./niil)"
