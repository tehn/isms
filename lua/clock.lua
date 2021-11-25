--- clock coroutines
-- @module clock

local clock = {}

clock.threads = {}

local clock_id_counter = 1
local function new_id()
  local id = clock_id_counter
  clock_id_counter = clock_id_counter + 1
  return id
end

--- create and start a coroutine using the norns clock scheduler.
-- @tparam function f coroutine body function
-- @param[opt] ... any extra arguments will be passed to the body function
-- @treturn integer coroutine handle that can be used with clock.cancel
-- @see clock.cancel
clock.run = function(f, ...)
  local coro = coroutine.create(f)
  local coro_id = new_id()
  clock.threads[coro_id] = coro
  clock.resume(coro_id, ...)
  return coro_id
end

--- stop execution of a coroutine previously started using clock.run.
-- @tparam integer coro_id coroutine handle
-- @see clock.run
clock.cancel = function(coro_id)
  isms.clock_cancel(coro_id)
  clock.threads[coro_id] = nil
end

local SCHEDULE_SLEEP = 0
local SCHEDULE_SYNC = 1

--- suspend execution of the calling coroutine and schedule resuming in specified time.
-- must be called from a coroutine previously started using clock.run.
-- @tparam float s seconds to wait for
clock.sleep = function(...)
  return coroutine.yield(SCHEDULE_SLEEP, ...)
end

--- suspend execution of the calling coroutine and schedule resuming at the next sync quantum of the specified value.
-- must be called from a coroutine previously started using clock.run.
-- @tparam float beat sync quantum. may be larger than 1
-- @tparam[opt] float offset if set, this value will be added to the sync quantum
clock.sync = function(...)
  return coroutine.yield(SCHEDULE_SYNC, ...)
end


-- todo: use c api instead
clock.resume = function(coro_id, ...)
  local coro = clock.threads[coro_id]

  local result, mode, time, offset = coroutine.resume(coro, ...)

  if coroutine.status(coro) == "dead" then
    if result then
      clock.cancel(coro_id)
    else
      error(mode)
    end
  else
    -- not dead
    if result and mode ~= nil then
      if mode == SCHEDULE_SLEEP then
        isms.clock_schedule_sleep(coro_id, time)
      elseif mode == SCHEDULE_SYNC then
        isms.clock_schedule_sync(coro_id, time, offset)
      else
        error('invalid clock scheduler mode')
      end
    end
  end
end


clock.cleanup = function()
  for id, coro in pairs(clock.threads) do
    if coro then
      clock.cancel(id)
    end
  end

  clock.transport.start = nil
  clock.transport.stop = nil
end

--- select the sync source.
-- @tparam string source "internal", "midi"
clock.set_source = function(source)
  if type(source) == "number" then
    isms.clock_set_source(util.clamp(source-1,0,1)) -- lua list is 1-indexed
  elseif source == "internal" then
    isms.clock_set_source(0)
  elseif source == "midi" then
    isms.clock_set_source(1)
  else
    error("unknown clock source: "..source)
  end
end

--- get current time in beats.
clock.get_beats = function()
  return isms.clock_get_time_beats()
end

--- get current tempo.
clock.get_tempo = function()
  return isms.clock_get_tempo()
end

--- get length of a single beat at current tempo in seconds.
clock.get_beat_sec = function()
  return 60.0 / clock.get_tempo()
end


clock.transport = {}

clock.transport.start = nil
clock.transport.stop = nil


clock.internal = {}

clock.internal.set_tempo = function(bpm)
  return isms.clock_internal_set_tempo(bpm)
end

clock.internal.start = function()
  return isms.clock_internal_start()
end

clock.internal.stop = function()
  return isms.clock_internal_stop()
end

clock.crow = {}

clock.crow.in_div = function(div)
  isms.clock_crow_in_div(div)
end


clock.midi = {}

isms.clock_start = function()
  if clock.transport.start ~= nil then
    clock.transport.start()
  end
end

isms.clock_stop = function()
  if clock.transport.stop ~= nil then
    clock.transport.stop()
  end
end


clock.help = [[
--------------------------------------------------------------------------------
clock.run( func )             start a new coroutine with function [func]
                              (returns) created id
clock.cancel( id )            cancel coroutine [id]
clock.sleep( time )           resume in [time] seconds
clock.sync( beats )           resume at next sync quantum of value [beats]
                                following to global tempo
clock.get_beats()             (returns) current time in beats
clock.get_tempo()             (returns) current tempo
clock.get_beat_sec()          (returns) length of a single beat at current
                                tempo in seconds
--------------------------------------------------------------------------------
-- example

-- start a clock with calling function [loop]
function init()
  clock.run(loop)
end

-- this function loops forever, printing at 1 second intervals 
function loop()
  while true do
    print("so true")
    clock.sleep(1)
  end
end
--------------------------------------------------------------------------------
]]

return clock
