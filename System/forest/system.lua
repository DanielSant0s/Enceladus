Screen.showSplash()

--Render.init()

local sky = Graphics.loadImage("System/forest/10_Sky.png")
local houses4 = Graphics.loadImage("System/forest/09_Forest.png")
local houses3 = Graphics.loadImage("System/forest/08_Forest.png")
local houses2 = Graphics.loadImage("System/forest/07_Forest.png")
local houses1 = Graphics.loadImage("System/forest/06_Forest.png")
local wall = Graphics.loadImage("System/forest/05_Particles.png")
local road = Graphics.loadImage("System/forest/04_Forest.png")
local cracks1 = Graphics.loadImage("System/forest/03_Particles.png")
local cracks2 = Graphics.loadImage("System/forest/02_Bushes.png")
local mist = Graphics.loadImage("System/forest/01_Mist.png")

local sky_pos = 0
local sky_pos2 = -639.5
local sky_speed = 0.05

local houses4_pos = 0
local houses4_pos2 = -639.5
local houses4_speed = 0.20

local houses3_pos = 0
local houses3_pos2 = -639.5
local houses3_speed = 0.35

local houses2_pos = 0
local houses2_pos2 = -639.5
local houses2_speed = 0.42

local houses1_pos = 0
local houses1_pos2 = -639.5
local houses1_speed = 0.50

local wall_pos = 0
local wall_pos2 = -639.5
local wall_speed = 0.60

local road_pos = 0
local road_pos2 = -639.5
local road_speed = 0.65

Font.fmLoad()

local freemem = System.getFreeMemory()
local freevram = System.getFreeVRAM()
local fps = System.getFPS()

function doSceneryLoop(sc_pos, sc_speed)
  sc_pos = sc_pos + sc_speed
  if (sc_pos >= 639.5) then 
    sc_pos = -639.5
  end
  return sc_pos, sc_speed
end

local t = 0

while true do
  Screen.clear()
  Graphics.drawImage(sky, sky_pos, 0)
  sky_pos, sky_speed = doSceneryLoop(sky_pos, sky_speed)

  Graphics.drawImage(sky, sky_pos2, 0)
  sky_pos2, sky_speed = doSceneryLoop(sky_pos2, sky_speed)

  Graphics.drawImage(houses4, houses4_pos, 0)
  houses4_pos, houses4_speed = doSceneryLoop(houses4_pos, houses4_speed)

  Graphics.drawImage(houses4, houses4_pos2, 0)
  houses4_pos2, houses4_speed = doSceneryLoop(houses4_pos2, houses4_speed)

  Graphics.drawImage(houses3, houses3_pos, 0)
  houses3_pos, houses3_speed = doSceneryLoop(houses3_pos, houses3_speed)

  Graphics.drawImage(houses3, houses3_pos2, 0)
  houses3_pos2, houses3_speed = doSceneryLoop(houses3_pos2, houses3_speed)

  Graphics.drawImage(houses2, houses2_pos, 0)
  houses2_pos, houses2_speed = doSceneryLoop(houses2_pos, houses2_speed)

  Graphics.drawImage(houses2, houses2_pos2, 0)
  houses2_pos2, houses2_speed = doSceneryLoop(houses2_pos2, houses2_speed)

  Graphics.drawImage(houses1, houses1_pos, 0)
  houses1_pos, houses1_speed = doSceneryLoop(houses1_pos, houses1_speed)

  Graphics.drawImage(houses1, houses1_pos2, 0)
  houses1_pos2, houses1_speed = doSceneryLoop(houses1_pos2, houses1_speed)

  Graphics.drawImage(wall, wall_pos, 0)
  wall_pos, wall_speed = doSceneryLoop(wall_pos, wall_speed)

  Graphics.drawImage(wall, wall_pos2, 0)
  wall_pos2, wall_speed = doSceneryLoop(wall_pos2, wall_speed)

  Graphics.drawImage(road, road_pos, 0)
  road_pos, road_speed = doSceneryLoop(road_pos, road_speed)

  Graphics.drawImage(road, road_pos2, 0)
  road_pos2, road_speed = doSceneryLoop(road_pos2, road_speed)
  
  Graphics.drawImage(cracks1, road_pos, 0)
  road_pos, road_speed = doSceneryLoop(road_pos, road_speed)

  Graphics.drawImage(cracks1, road_pos2, 0)
  road_pos2, road_speed = doSceneryLoop(road_pos2, road_speed)

  Graphics.drawImage(cracks2, road_pos, 0)
  road_pos, road_speed = doSceneryLoop(road_pos, road_speed)

  Graphics.drawImage(cracks2, road_pos2, 0)
  road_pos2, road_speed = doSceneryLoop(road_pos2, road_speed)

  Graphics.drawImage(mist, 0, 0)

  if t > 240 then
    fps = System.getFPS()
    t = 0
  end
  
  Font.fmPrint(30, 355, 0.6, "\n" .. fps .. " FPS\n")
  Font.fmPrint(30, 380, 0.6, "\nFree RAM: " .. freemem .. " bytes\n")
  Font.fmPrint(30, 405, 0.6, "\nFree VRAM: " .. freevram .. " KiB\n")

  --Render.renderModel()
  Screen.waitVblankStart()
  Screen.flip()
end
