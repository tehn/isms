function key(x) print('key: '..x) end
function osc.receive(path, args, from) print('osc: '..path) end
function metro.tick(i,stage) print('metro: '..i..' '..stage) end
function grid.key(x,y,z) print('grid: '..x..' '..y..' '..z) end
