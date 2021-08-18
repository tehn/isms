# isms

lua + osc + midi + grid + sdl

- build: `make`
- run: `build/isms`

requires lua 5.4, sdl2

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile

`make core` converts `src/core.lua` to `src/core.h` using `sed`

## currently/FIXME

`example.lua` is run at startup, which accepts sdl key input to place a random white pixel.

quit by closing sdl window.


## TODO

- lua glue
  - naming conventions: event/callbacks
- libmonome
- clocks
- midi
- external command entry (ie websocket)
- repl
- lua error checking
  - clean up lua.c
- drawing lib
- optimization
  - sdl should probably go in its own thread


## links

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
