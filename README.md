# isms

lua + osc + midi + grid + sdl

- build: `make`
- install: `sudo make install`
- run: `isms`

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
  map <C-\> :silent .w !xargs -0 echo -n > /dev/udp/localhost/11001<CR>
- specifying a filename as an argument will run that instead of `example.lua` (ie, if your working directory is elsewhere).


## design

reconstruction of norns. reconsidering design for use on a computer with large screen and keyboard.


## structure

library folder: `~/.local/share/isms`

`system` subfolder is copied by `make install`. any user libraries can go directly in `~/local/share/isms`.


## TODO

- socket input, allow blocks, not just line
- lua
  - naming conventions (callbacks)
  - utils: sys.time
  - make script/path var available in lua
  - init system with pre/post init, deinit/cleanup pre/post
    - anticipate mods (?)
- abandon repl in favor of socket style? (just because it's incomplete and maybe not worth it)

- sdl
  - window management
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
window.pixel(x,y,color)
window.line(x1,y1,x2,y2,color)
window.redraw()

metro.start(index, time_sec, count, stage)
metro.stop(index)

g = grid.connect()
g.all(z)
g.led(x,y,z)
g.redraw()

m = midi.connect() -- more TODO

-- events
metro.tick(index, stage)
window.key(code)
osc.receive(path, args, from)
g.key(x,y,z)
m.receive -- TODO
```


## acknowledgements

- based heavily on `matron` from norns, written by @catfact
- sdl use patterned on work by @neauoire
- additional thanks to @artem, @ngwese, and @csboling


## reference

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
- http://mech.math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html
- https://hdante.wordpress.com/2014/07/08/the-hello-wayland-tutorial/
