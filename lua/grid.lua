local Grid = {}
Grid.__index = Grid

Grid.devices = {}

function Grid.new(id, serial, name, dev)
  local g = setmetatable({}, Grid)

  g.id = id
  g.serial = serial
  g.name = name.." "..serial
  g.dev = dev
  g.key = nil
  g.remove = nil
  g.rows = isms.grid_rows(dev)
  g.cols = isms.grid_cols(dev)
  g.port = nil

  return g
end

--- static callback when any grid device is added;
-- user scripts can redefine
-- @static
-- @param dev : a Grid table
function Grid.add(dev)
  print("grid added:", dev.id, dev.name, dev.serial)
end

--- static callback when any grid device is removed;
-- user scripts can redefine
-- @static
-- @param dev : a Grid table
function Grid.remove(dev) end

-- set grid rotation.
-- @tparam integer val : rotation 0,90,180,270 as [0, 3]
function Grid:rotation(val)
  isms.grid_set_rotation(self.dev, val)
end

--- set state of single LED on this grid device.
-- @tparam integer x : column index (1-based!)
-- @tparam integer y : row index (1-based!)
-- @tparam integer val : LED brightness in [0, 15]
function Grid:led(x, y, val)
  isms.grid_set_led(self.dev, x, y, val)
end

--- set state of all LEDs on this grid device.
-- @tparam integer val : LED brightness in [0, 15]
function Grid:all(val)
  isms.grid_all_led(self.dev, val)
end

--- update any dirty quads on this grid device.
function Grid:refresh()
  isms.monome_refresh(self.dev)
end

--- intensity
function Grid:intensity(i)
  isms.monome_intensity(self.dev, i)
end

isms.grid = {}

-- grid add
isms.grid.add = function(id, serial, name, dev)
  local g = Grid.new(id,serial,name,dev)
  Grid.devices[id] = g
  if Grid.add ~= nil then Grid.add(g) end
end

-- grid remove
isms.grid.remove = function(id)
  local g = Grid.devices[id]
  if g then
    if Grid.remove then
      Grid.remove(Grid.devices[id])
    end
  end
  Grid.devices[id] = nil
end

-- redefine global grid key input handler
isms.grid.key = function(id, x, y, s)
  local g = Grid.devices[id]
  if g ~= nil then
    if g.key ~= nil then
      g.key(x, y, s)
    end
  end
end

Grid.help = [[
--------------------------------------------------------------------------------
grid.connect( port )          create a grid table using device [port]
                                default [port] 1 if unspecified
                              (returns) grid table
.key( x, y, z )               function called with incoming grid key event
                                this should be redefined by the script
.led( x, y, level )           set LED at [x,y] to [level]
                                [level] range is 0..15
.all( level )                 set all grid LED to [level]
                                [level] range is 0..15
.refresh()                    update the grid LED state

--------------------------------------------------------------------------------
-- example

lx,ly,lz = 0,0,0

-- connect grid
g = grid.connect()

-- key function
g.key = function(x,y,z)
  print(x,y,z)
  lx = x
  ly = y
  lz = z*15
  draw_grid()
end

-- simple draw function
draw_grid()
  g.all(0)
  g.led(lx,ly,lz)
  g.refresh()
end
--------------------------------------------------------------------------------
]]      

return Grid
