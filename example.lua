--- script
print("hello there")
a = 8
b = 3
print(a+b)

for i=1,200 do
  screen.pixel(10+i,20,0xFFFFFF-i);
end

screen.redraw()

key = function(x)
  screen.pixel(math.random(200),math.random(200),0xFFFFFF);
  screen.redraw()
  osc.send({"localhost",7770},"/key",{x})
end
