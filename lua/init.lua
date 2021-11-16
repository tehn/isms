function key(x) print('key: '..x) end
function osc.receive(path, args, from) print('osc: '..path) end
function metro.tick(i,stage) print('metro: '..i..' '..stage) end
function grid.key(i,x,y,z) print('grid['..i..']: '..x..' '..y..' '..z) end

midi = dofile('lua/midi.lua')
grid = dofile('lua/grid.lua')


