local home = os.getenv('HOME')
package.path = home..'/.local/share/isms/?.lua;'..package.path
package.path = home..'/.local/share/isms/system/?.lua;'..package.path
print("lua init >> package.path: "..package.path)

tab = require('tabutil')
midi = require('midi')
grid = require('grid')

function window.key(x) print('window key: '..x) end
function osc.receive(path, args, from) print('osc: '..path) end
function metro.tick(i,stage) print('metro: '..i..' '..stage) end

