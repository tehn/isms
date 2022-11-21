# isms

lua + osc + grid + midi + crow + sdl + clocks

- build: `make`
- install: `sudo make install`
- run: `isms example.lua`

requires lua 5.4, sdl2, libevdev, libmonome

only tested on linux (ubuntu), include/lib paths are hardcoded in makefile


## currently

- install with `make; sudo make install` then run with `isms example.lua` 
- `example.lua` is run at startup, creating a new window which accepts key input to place a random pixel line and send an OSC message to supercollider
- `example.scd` will provide a rudimentary osc-listening synth for testing OSC
- if found, connects to grid at `/dev/ttyACM0` and sends OSC on key input
- a rudimentary REPL is implemented, try `print('hello')`
- ctrl-q quits (or close sdl window)
- ctrl-r reloads lua script
- remote lua input via UDP on port 11001 ie `echo -n "print('hello')" > /dev/udp/localhost/11001` (set up your editor to send to this port)
- .vimrc map:
  map <C-\> :silent .w !xargs -0 echo -n > /dev/udp/localhost/11001<CR>


## structure

library folder: `/usr/local/share/isms`

`system` subfolder is copied by `make install`. any user libraries can go directly in `/usr/local/share/isms`.


## TODO
```
- lua
  - naming conventions (callbacks) --- event table?
- sdl
  - text
  - more drawing functions
  - mouse events (?)

- socket input: allow blocks, not just line
- repl history (readline/etc)
- repl: fix color coding (where text is coming from)
- refine device management (remove vports)
- crow
- metro allocator (?)
- makefile improvements
- optimization
  - sdl should probably have its own thread
- security: should udp socket input check incoming ip (restrict to localhost?)
  - or arg to disable
```


## lua

```
window.init(x,y)
window.pixel(x,y,color)
window.line(x1,y1,x2,y2,color)
window.redraw()

metro.start(index, time_sec, count, stage)
metro.stop(index)

clock -- TODO

g = grid.connect()
g:all(z)
g:led(x,y,z)
g:redraw()

m = midi.connect() -- more TODO

-- events
metro.tick(index, stage)
window.key(code)
osc.receive(path, args, from)
g.key(x,y,z)
m.receive -- TODO
```


## acknowledgements

reconstruction of norns. reconsidering design for use on a computer with large screen and keyboard.

- based heavily on `matron` from norns, written by @catfact
- sdl use patterned on work by @neauoire
- additional thanks to @artem (clocks), @ngwese, and @csboling


## reference

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
- http://mech.math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html
- https://hdante.wordpress.com/2014/07/08/the-hello-wayland-tutorial/
