--- script
print("example.lua -------------> hello there")

for i=0,255 do
  screen.pixel(i,4,0xFFFF00-i);
  screen.pixel(i,8,0x00FFFF-i);
  screen.pixel(i,12,0xFF00FF-i);
end

screen.redraw()

x1,y1,x2,y2,c = 128,80,0,0,0xffffff

key = function(x)
  x2 = x1
  y2 = y1
  x1 = math.random(192)+32
  y1 = math.random(64)+32
  c = c - 0x111111
  screen.line(x1,y1,x2,y2,c)
  screen.redraw()
  osc.send({"localhost",57120},"/n",{x%127})
  print("key: "..x)
end

metro.tick = function(i,s)
  print("metro",i,s)
  grid.all(s)
  grid.redraw()
end

metro.start(1,0.1,5,0);

grid.key = function(x,y,z)
  print("grid",x,y,z)
  osc.send({"localhost",57120},"/n",{(7-y)*5+x+30})
  grid.led(x,y,15);
  grid.redraw();
end

dofile("test.lua")
