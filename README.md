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


## TODO

- lots of glue
- libmonome
- metro/clocks
- osc
- midi
- external command entry (ie websocket)
- repl


## links

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
