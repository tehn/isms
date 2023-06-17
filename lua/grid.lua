grid.lookup = {}

function grid.connect(n)
	n = n or 0
	local g = {}
	function g.all(z) grid.all(n,z) end
	function g.led(x,y,z) grid.led(n,x,y,z) end
	function g.redraw() grid.redraw(n) end
	function g.all(z) grid.all(n,z) end
	g.event = {}
	function g.event.key(x,y,z) print("grid key",x,y,z) end
	grid.lookup[n] = g
	return g
end

function grid.event.key(n,x,y,z)
	if grid.lookup[n] then
		grid.lookup[n].event.key(x,y,z)
	end
end
