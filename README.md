# niil (untitled/wip)

lua + osc + midi + grid + sdl

- build: `make`
- run: `build/niil`

requires lua 5.4, sdl2

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile

`make core` converts `src/core.lua` to `src/core.h` using `sed`

## currently/FIXME

`example.lua` is run at startup, which accepts sdl key input to place a random white pixel.

quit by closing sdl window.

quit will possibly hang at various `SDL_Destroy` methods and need manual killing

SDL events are not thread-safe: https://github.com/Rust-SDL2/rust-sdl2/issues/318 and https://forums.libsdl.org/viewtopic.php?t=5720&sid=07b5c57e4932124f7c2c51a55e308e7d


## TODO

- lots of glue
- libmonome
- metro/clocks
- osc
- midi
- external command entry (ie websocket)
- repl
