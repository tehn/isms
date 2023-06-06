function help(topic)
  local help_topics = "help(topic): clock, grid"
  if topic == nil then
    print(help_topics)
  elseif type(topic)=="table" then
    if topic.help then
      print(topic.help)
    end
  end
end

grid.help = [[
--------------------------------------------------------------------------------
grid.led( id, x, y, level )        set LED at [x,y] to [level]
grid.all( id, level )              set all grid LED to [level]
grid.redraw( id )                  update the grid LED state

grid.key( id, x, y, z )            function called with incoming grid key event
--------------------------------------------------------------------------------
]]      
