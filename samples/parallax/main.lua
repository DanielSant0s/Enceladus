--Screen.showSplash()

local sky = Graphics.loadImage("samples/parallax/sky.png")
local houses4 = Graphics.loadImage("samples/parallax/houses4.png")
local houses3 = Graphics.loadImage("samples/parallax/houses3.png")
local houses2 = Graphics.loadImage("samples/parallax/houses2.png")
local houses1 = Graphics.loadImage("samples/parallax/houses1.png")
local wall = Graphics.loadImage("samples/parallax/wall.png")
local road = Graphics.loadImage("samples/parallax/road.png")
local cracks1 = Graphics.loadImage("samples/parallax/cracks1.png")
local cracks2 = Graphics.loadImage("samples/parallax/cracks2.png")


Font.fmLoad()

local freemem = System.getFreeMemory()
local freevram = System.getFreeVRAM()

local sky_pos = 0
local sky_pos2 = -640
local sky_speed = 0.05

local houses4_pos = 0
local houses4_pos2 = -640
local houses4_speed = 0.20

local houses3_pos = 0
local houses3_pos2 = -640
local houses3_speed = 0.35

local houses2_pos = 0
local houses2_pos2 = -640
local houses2_speed = 0.42

local houses1_pos = 0
local houses1_pos2 = -640
local houses1_speed = 0.50

local wall_pos = 0
local wall_pos2 = -640
local wall_speed = 0.60

local road_pos = 0
local road_pos2 = -640
local road_speed = 0.65

function doSceneryLoop(sc_pos, sc_speed)
  sc_pos = sc_pos + sc_speed
  if (sc_pos >= 640) then 
    sc_pos = -640
  end
  return sc_pos, sc_speed
end

local time = Timer.new()
local prev = 0
local cur = 0

while true do
  Screen.clear()

  Graphics.drawImage(sky, sky_pos, -32)
  sky_pos, sky_speed = doSceneryLoop(sky_pos, sky_speed)

  Graphics.drawImage(sky, sky_pos2, -32)
  sky_pos2, sky_speed = doSceneryLoop(sky_pos2, sky_speed)

  Graphics.drawImage(houses4, houses4_pos, -32)
  houses4_pos, houses4_speed = doSceneryLoop(houses4_pos, houses4_speed)

  Graphics.drawImage(houses4, houses4_pos2, -32)
  houses4_pos2, houses4_speed = doSceneryLoop(houses4_pos2, houses4_speed)

  Graphics.drawImage(houses3, houses3_pos, -32)
  houses3_pos, houses3_speed = doSceneryLoop(houses3_pos, houses3_speed)

  Graphics.drawImage(houses3, houses3_pos2, -32)
  houses3_pos2, houses3_speed = doSceneryLoop(houses3_pos2, houses3_speed)

  Graphics.drawImage(houses2, houses2_pos, -32)
  houses2_pos, houses2_speed = doSceneryLoop(houses2_pos, houses2_speed)

  Graphics.drawImage(houses2, houses2_pos2, -32)
  houses2_pos2, houses2_speed = doSceneryLoop(houses2_pos2, houses2_speed)

  Graphics.drawImage(houses1, houses1_pos, -32)
  houses1_pos, houses1_speed = doSceneryLoop(houses1_pos, houses1_speed)

  Graphics.drawImage(houses1, houses1_pos2, -32)
  houses1_pos2, houses1_speed = doSceneryLoop(houses1_pos2, houses1_speed)

  Graphics.drawImage(wall, wall_pos, -32)
  wall_pos, wall_speed = doSceneryLoop(wall_pos, wall_speed)

  Graphics.drawImage(wall, wall_pos2, -32)
  wall_pos2, wall_speed = doSceneryLoop(wall_pos2, wall_speed)

  Graphics.drawImage(road, road_pos, -32)
  road_pos, road_speed = doSceneryLoop(road_pos, road_speed)

  Graphics.drawImage(road, road_pos2, -32)
  road_pos2, road_speed = doSceneryLoop(road_pos2, road_speed)

  Graphics.drawImage(cracks1, road_pos, -32)
  road_pos, road_speed = doSceneryLoop(road_pos, road_speed)

  Graphics.drawImage(cracks1, road_pos2, -32)
  road_pos2, road_speed = doSceneryLoop(road_pos2, road_speed)

  Graphics.drawImage(cracks2, road_pos, -32)
  road_pos, road_speed = doSceneryLoop(road_pos, road_speed)

  Graphics.drawImage(cracks2, road_pos2, -32)
  road_pos2, road_speed = doSceneryLoop(road_pos2, road_speed)

  prev = cur
  cur = Timer.getTime(time)

  local fps = System.getFPS(prev, cur)
  Font.fmPrint(30, 355, 0.6, "\n" .. fps .. " FPS\n")
  Font.fmPrint(30, 380, 0.6, "\nFree RAM: " .. freemem .. " bytes\n")
  Font.fmPrint(30, 405, 0.6, "\nFree VRAM: " .. freevram .. " KiB\n")
  
  Screen.waitVblankStart()
  Screen.flip()
end
