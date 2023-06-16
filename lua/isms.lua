--package.path = '/usr/local/share/isms/?.lua;'..package.path
--package.path = '/usr/local/share/isms/system/?.lua;'..package.path
local home = os.getenv('HOME')
package.path = home..'/.local/share/isms/?.lua;'..package.path
package.path = home..'/.local/share/isms/system/?.lua;'..package.path
--print("lua init >> package.path: "..package.path)

--function window.key(x) print('window key: '..x) end
function osc.receive(path, args, from) print('osc: '..path) end
function metro.tick(i,stage) print('metro: '..i..' '..stage) end

tab = require('tabutil')
--midi = require('midi')
clock = require('clock')
require('help')

isms.state = {}

function isms.clear()
  print(">> isms.clear")
  local state = {}
  setmetatable(_G, {
    __index = function (_,k) return state[k] end,
    __newindex = function(_,k,v) state[k] = v end,
  })
  clock.clear()
  metro.clear()
end

function isms.run(file)
  isms.clear()
  if file then isms.state.file = file end
  if isms.state.file then
    dofile(isms.state.file)
  end
end

-- user config
local config = home..'/.local/share/isms/init.lua'
local f=io.open(config,"r")
if f~=nil then
	io.close(f)
	dofile(config)
end
