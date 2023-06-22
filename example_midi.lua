--- script

window.init(256,128)
window.clear()

for i=0,255 do
  window.pixel(i,4,0xFFFF00-i)
  window.pixel(i,8,0x00FFFF-i)
  window.pixel(i,12,0xFF00FF-i)
end

window.redraw()

midi.event = function(channel, type, data1, data2)
  if type == midi.types.note_on then
    print('midi note_on', channel, data1, data2)
  elseif type == midi.types.note_off then
    print('midi note_off', channel, data1)
  else
    print('midi other', channel, type, data1, data2)
  end
end

midi.send(1, midi.types.note_on, 48, 127)

dofile("test.lua")

--m = midi.connect()
--m.event = function(d) tab.print(d) end

--w = midi.connect(2)
--[[
w:note_on(60,100)
w:note_off(60,100)
]]--

