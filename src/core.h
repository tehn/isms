const char* core = " \
print('this is core.lua') \
 \
function key(x) \
  print('key: '..x) \
end \
 \
function osc.receive(path, args, from) \
  print('osc: '..path) \
end \
 \
function metro.tick(i,stage) \
  print('metro: '..i..' '..stage) \
end \
";
