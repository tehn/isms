package.path = './lua/?.lua;'..package.path
print("lua init >> package.path: "..package.path)

print("REQ")

tab = require('tabutil')
midi = require('midi')
grid = require('grid')

function key(x) print('key: '..x) end
function osc.receive(path, args, from) print('osc: '..path) end
function metro.tick(i,stage) print('metro: '..i..' '..stage) end
--function grid.key(i,x,y,z) print('grid['..i..']: '..x..' '..y..' '..z) end

