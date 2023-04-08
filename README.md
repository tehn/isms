# isms

lua + clocks + osc + grid + midi + ui

- build: `make`
- install: `sudo make install`
- run: `isms example.lua`

requires lua 5.4, alsa, sdl2, libevdev, libmonome

apt-get: liblua5.4-dev libasound2-dev libsdl2-dev libevdev-dev

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
- cpu usage at rest
- linux-macos compatibility
  x grid: use serialosc instead of libmonome (removes udev)
  - midi: use portmidi https://github.com/PortMidi/PortMidi
	- clock_nanosleep
x refine device management (remove vports)
- consider implications of running multiple instances
  x osc port assignments (ie auto-increment if requested is taken)
  - grid/midi "focus"
- naming convention for event handlers
  x function event.grid.key(x,y,z)
	x done: grid, metro
	- todo: clock, sdl
- config file (pre-run) for setting "reserved" grid/midi slots (ie serial numbers)
- makefile improvements
  - consider cmake

- sdl
  - text
  - more drawing functions
  - mouse events (?)
- socket input: allow blocks, not just line
- repl history (readline/etc)
- repl: fix color coding (where text is coming from)

- optimization
  - sdl should probably have its own thread
- security: should udp socket input check incoming ip (restrict to localhost?)
  - or arg to disable

- metro allocator (?)
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

grid.all(id,z)
grid.led(id,x,y,z)
grid.redraw(id)

-- MIDI

-- events
window.key(code)
osc.receive(path, args, from)

-- events
event.metro.tick(index, stage)
event.window.key(code)
event.osc.receive(...)
event.grid.key(id,x,y,z)
event.grid.add(id,serial)
event.grid.remove(id,serial)
event.midi.noteon(id,note,vel,ch)
event.midi.add(id,...)
```


## acknowledgements

reimagining of norns. reconsidering design for use on a computer with large screen and keyboard.

- based heavily on `matron` from norns, written by @catfact
- sdl use patterned on work by @neauoire
- additional thanks to @artem (clocks), @ngwese, and @csboling


## reference

- https://www.lua.org/manual/5.4/manual.html#4
- https://github.com/klassmann/sdl2-lua53-example/blob/master/src/main.c
- https://lucasklassmann.com/blog/2019-02-02-how-to-embeddeding-lua-in-c/#running-lua-code
- http://mech.math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html
- https://hdante.wordpress.com/2014/07/08/the-hello-wayland-tutorial/
