grid = {}
grid.serial = {}  -- list of serial numbers, point to device assignment index
grid.id = {}      -- list of device id, point to device assignment index
grid.device = {}  -- list of device pointers by assignment

-- these get user overwritten
function grid.key(id, x, y, z) print("grid.key",id,x,y,z) end
function grid.add(id) print("grid.add",id) end
function grid.remove(id) print("grid.remove",id) end

function grid.led(id, x, y, level) isms.grid_led(isms.grid.device[id], x, y, level) end
function grid.all(id, level) isms.grid_all(isms.grid.device[id], level) end
function grid.redraw(id) isms.grid_redraw(isms.grid.device[id]) end
function grid.intensity(id, i) isms.monome_intensity(isms.grid.device[id], i) end
function grid.rotation(id, v) isms.grid_set_rotation(isms.grid.device[id], v) end

grid.help = [[
--------------------------------------------------------------------------------
grid.led( id, x, y, level )          set LED at [x,y] to [level]
grid.all( id, level )                set all grid LED to [level]
grid.redraw( id )                    update the grid LED state

grid.key( id, x, y, z )              function called with incoming grid key event
--------------------------------------------------------------------------------
]]      

isms.grid = {}

-- grid add
isms.grid.add = function(id, serial, name, dev)
  local e = grid.serial[serial]
  if e then -- existing serial
    grid.device[e] = dev
    grid.id[id] = e
    --print("existing serial "..e);
    grid.add(e)
  else -- new entry
    local n = #grid.device + 1
    grid.device[n]= dev
    grid.id[id] = n
    grid.serial[serial] = n
    --print("new index "..n);
    grid.add(n)
  end
end

-- grid remove
isms.grid.remove = function(id)
  local n = grid.id[id]
  local g = grid.device[n]
  grid.device[n] = nil
  grid.remove(grid.id[id])
end

-- grid key input handler
isms.grid.key = function(id, x, y, s)
  grid.key(grid.id[id],x,y,s)
end

return grid
