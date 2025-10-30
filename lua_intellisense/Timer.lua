---@meta
---@file intellisense metadata corresponding to the `Timer` library inside `src/lua/timer.cpp`
---@diagnostic disable


---@class Timer
Timer = {}

---@alias timer integer

--- Creates a new timer
--- @return timer T
function Timer.new() end

--- @param T timer the timer to use
--- @return integer R
function Timer.getTime(T) end

---@param T timer 
---@param val integer the new time
function Timer.setTime(T, val) end

--- destroys the timer
---@param T timer 
function Timer.destroy(T) end

--- pauses the timer
---@param T timer 
function Timer.pause(T) end

--- resumes the timer
---@param T timer 
function Timer.resume(T) end

--- resets the timer
---@param T timer 
function Timer.reset(T) end

--- Checks if timer is playing
---@param T timer 
---@return boolean playing if the timer is playing
function Timer.isPlaying(T) end
