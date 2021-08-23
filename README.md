# isms

lua + osc + midi + grid + sdl

- build: `make`
- run: `build/isms`

requires lua 5.4, sdl2

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile

`make core` converts `src/core.lua` to `src/core.h` using `sed`

## currently

- `example.lua` is run at startup, which accepts sdl key input to place a random white pixel and send an OSC message to supercollider
- `example.scd` will provide a rudimentary osc-listening synth for testing.
- if found, connects to grid at `/dev/ttyACM0` and sends OSC on key input.
- quit by closing sdl window.

## design

attempting to stay as minimal as possible


## TODO

- lua
  - naming conventions: event/callbacks
  - script reload
  - utils: sys.time, sys.cmd
- arg filename (or, figure out file use pattern)
  - make path var available in lua
- repl (ncurses?)
- external command entry (websocket? osc?)
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

(and)

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

grid.all(z)
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


## sketch

1. core
2. ~/isms/settings.lua
3. (script)
4. pre_init(); init(); post_init();
5. ...
6. deinit(); post_deinit();
