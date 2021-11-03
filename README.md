# isms

lua + osc + midi + grid + sdl

- build: `make`
- run: `build/isms`

requires lua 5.4, sdl2

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile

`make core` converts `src/core.lua` to `src/core.h` using `sed`

## currently

- `example.lua` is run at startup, creating a new window which accepts sdl key input to place a random white pixel and send an OSC message to supercollider
- `example.scd` will provide a rudimentary osc-listening synth for testing.
- if found, connects to grid at `/dev/ttyACM0` and sends OSC on key input.
- a rudimentary REPL is implemented, try `print('hello')`
- quit by closing sdl window or ctrl-q
- ctrl-r reloads lua script

specifying a filename as an argument will run that instead of `example.lua` (ie, if your working directory is elsewhere).

## design

attempting to stay as minimal as possible

`core.lua` is included in the compiled binary so we don't need to have a library path for essential lua functionality. though in the future it makes sense to have some default locations to look for user libs.

## TODO

- repl improvements (readline/ncurses?)
- device management
  - hotswap detection of grids/midi/etc (udev) --- see norns device
- midi
- lua
  - naming conventions
    - consider putting all callbacks under "event" table
  - utils: sys.time, sys.cmd, reload
  - make script/path var available in lua
  - init system with pre/post init, deinit/cleanup pre/post
    - anticipate mods (?)

- sdl
  - window management
    - resize enable and management
    - default screen size and pixel scaling
      - should this be global (config file?) or script-defined?
  - drawing lib
    - lines/curves/etc
    - text (libSDL2_ttf)
  - mouse events

- external command entry (websocket? osc?)
- clocks
- metro allocator (?)

(and)

- lib management and extensibility: suggested best practices, folder structure/etc
- sane makefile
- optimization
  - sdl should probably have its own thread
- drop sdl for libx11?


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


## reference

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
- http://mech.math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html
