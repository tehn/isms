# isms

lua + osc + midi + grid + sdl

- build: `make`
- run: `build/isms`

requires lua 5.4, sdl2

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile

`make core` converts `src/core.lua` to `src/core.h` using `sed`

## currently

- `example.lua` is run at startup, which accepts sdl key input to place a random white pixel.
- if found, connects to grid at `/dev/ttyACM0'
- quit by closing sdl window.
- `example.scd` will provide a rudimentary osc-listening synth for testing.


## TODO

- lua
  - error checking (clean up lua.c)
  - include/etc
  - script reload
  - naming conventions: event/callbacks
- external command entry (websocket? osc?)
- repl (ncurses?)
- window management
  - quit key (?)
  - resize enable and management
  - default screen size and pixel scaling
- drawing lib
  - lines
  - text
- clocks
- midi
- metro allocator (?)

- sane makefile
- device management
  - proper detection of grids/etc (udev)
- optimization
  - sdl should probably have its own thread


## lua

```
screen.pixel(x,y,color)
screen.redraw()

metro.start(index, time_sec, count, stage)
metro.stop(index)

grid.led(x,y,z)
grid.redraw()

-- events
metro.tick(index, stage)
key(code)
grid.key(x,y,z)
osc.receive(path, args, from)
```


## links

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
