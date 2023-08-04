
if doesFileExist("System/index.lua") then
	dofile("System/index.lua");
  elseif doesFileExist("System/script.lua") then
	dofile("System/script.lua");
  elseif doesFileExist("System/system.lua") then
	dofile("System/system.lua");
  elseif doesFileExist("index.lua") then
	dofile("index.lua");
  elseif doesFileExist("script.lua") then
	dofile("script.lua");
  elseif doesFileExist("system.lua") then
	dofile("system.lua");
end