# isms

lua + osc + midi + grid + sdl

- build: `make`
- run: `build/isms`

requires lua 5.4, sdl2, libevdev

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile

## currently

- `example.lua` is run at startup, creating a new window which accepts key input to place a random pixel line and send an OSC message to supercollider
- `example.scd` will provide a rudimentary osc-listening synth for testing OSC
- if found, connects to grid at `/dev/ttyACM0` and sends OSC on key input
- a rudimentary REPL is implemented, try `print('hello')`
- ctrl-q quits (or close sdl window)
- ctrl-r reloads lua script
- remote lua input via UDP on port 11001 ie `echo -n "print('hello')" > /dev/udp/localhost/11001` (set up your editor to send to this port)
- .vimrc map:
  map <C-\> :silent .w !xargs echo -n > /dev/udp/localhost/11001<CR>
- specifying a filename as an argument will run that instead of `example.lua` (ie, if your working directory is elsewhere).

## design

attempting to stay as minimal as possible


## structure

## TODO

- device management, hotswap detection of grids/midi/etc (udev) --- see norns device
- midi
- lua
  - naming conventions
  - utils: sys.time
  - make script/path var available in lua
  - init system with pre/post init, deinit/cleanup pre/post
    - anticipate mods (?)
- abandon repl in favor of socket style? (just because it's incomplete and maybe not worth it)

- sdl
  - window management
    - auto-scale according to resize
    - default pixel counts? (script redefined?)
  - drawing lib
    - lines/curves/etc
    - text
  - mouse events
- clocks
- metro allocator (?)

(and)

- makefile improvements
- lib management and extensibility: suggested best practices, folder structure/etc
- optimization
  - sdl should probably have its own thread
- should udp socket input check incoming ip (restrict to localhost?)


## lua

```
screen.pixel(x,y,color)
screen.line(x1,y1,x2,y2,color)
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
- https://hdante.wordpress.com/2014/07/08/the-hello-wayland-tutorial/
