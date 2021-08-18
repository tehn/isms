--- script
print("example.lua -------------> helllo there")

for i=0,255 do
  screen.pixel(i,10,0xFFFFFF-i);
  screen.pixel(i,20,0x00FFFF-i);
  screen.pixel(i,30,0xFF00FF-i);
end

screen.redraw()

key = function(x)
  screen.pixel(math.random(256),math.random(16)+50,0xFFFFFF);
  screen.redraw()
  osc.send({"localhost",7770},"/key",{x})
end

metro.tick = function(i,s) print("metro",i,s); end

metro.start(1,1,3,0);

grid.key = function(x,y,z)
  print("grid",x,y,z)
  grid.led(x,y,15);
  grid.redraw();
end
