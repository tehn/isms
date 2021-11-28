local home = os.getenv('HOME')
package.path = home..'/.local/share/isms/?.lua;'..package.path
package.path = home..'/.local/share/isms/system/?.lua;'..package.path
--print("lua init >> package.path: "..package.path)

tab = require('tabutil')
midi = require('midi')
grid = require('grid')
clock = require('clock')

function window.key(x) print('window key: '..x) end
function osc.receive(path, args, from) print('osc: '..path) end
function metro.tick(i,stage) print('metro: '..i..' '..stage) end


isms.state = {}

function isms.clear()
  print(">> isms.clear")
  local state = { }
  setmetatable(_G, {
    __index = function (_,k)
      return state[k]
    end,
    __newindex = function(_,k,v)
      state[k] = v
    end,
  })
  clock.cleanup()
  -- TODO STOP METROS
end

function isms.run(file)
  isms.clear()
  if file then isms.state.file = file end
  if isms.state.file then
    dofile(isms.state.file)
  end
end

