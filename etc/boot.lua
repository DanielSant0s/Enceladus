
if System.doesFileExist("System/index.lua") then
	dofile("System/index.lua");
  elseif System.doesFileExist("System/script.lua") then
	dofile("System/script.lua");
  elseif System.doesFileExist("System/system.lua") then
	dofile("System/system.lua");
  elseif System.doesFileExist("index.lua") then
	dofile("index.lua");
  elseif System.doesFileExist("script.lua") then
	dofile("script.lua");
  elseif System.doesFileExist("system.lua") then
	dofile("system.lua");
end