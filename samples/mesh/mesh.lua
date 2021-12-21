Screen.setMode(NTSC, 640, 448, CT24, INTERLACED, FIELD, true, Z16S)

Render.init(4/3)

local dragontex = Graphics.loadImage("mesh/dragon.png")
Graphics.setImageFilters(dragontex, LINEAR) 
local dragonmesh = Render.loadOBJ("mesh/dragon.obj", dragontex)

local monkeytex = Graphics.loadImage("mesh/monkey.png")
Graphics.setImageFilters(monkeytex, LINEAR) 
local monkeymesh = Render.loadOBJ("mesh/monkey.obj", monkeytex)

Camera.position(0.0, 0.0, 50.0)
Camera.rotation(0.0, 0.0,  0.0)

Lights.create(4)

--Lights.set(1,  0.0,  0.0,  0.0, 1.0, 1.0, 1.0,     AMBIENT)
Lights.set(2,  1.0,  0.0, -1.0, 1.0, 1.0, 1.0, DIRECTIONAL)
--Lights.set(3,  0.0,  1.0, -1.0, 0.9, 0.5, 0.5, DIRECTIONAL)
--Lights.set(4, -1.0, -1.0, -1.0, 0.5, 0.5, 0.5, DIRECTIONAL)

local lx = nil
local ly = nil
local pad = nil
local oldpad = nil
local modeltodisplay = 0

local savedlx = 0.0
local savedly = 180.0

while true do

    oldpad = pad
    pad = Pads.get()
    lx, ly = Pads.getLeftStick()
    lx = lx / 1024.0
    ly = ly / 1024.0
    savedlx = savedlx - lx
    savedly = savedly - ly

    Screen.clear(Color.new(40, 40, 40, 128))

    
    if Pads.check(pad, PAD_LEFT) and not Pads.check(oldpad, PAD_LEFT) then
        modeltodisplay = modeltodisplay - 1
    end

    if Pads.check(pad, PAD_RIGHT) and not Pads.check(oldpad, PAD_RIGHT) then
      modeltodisplay = modeltodisplay + 1
    end

    if modeltodisplay > 1 then
        modeltodisplay = 0
    elseif modeltodisplay < 0 then
        modeltodisplay = 1
    end

    if modeltodisplay == 0 then Render.drawOBJ(dragonmesh, 0.0, 0.0, 30.0, savedly, savedlx, 0.0) end
    if modeltodisplay == 1 then Render.drawOBJ(monkeymesh, 0.0, 0.0, 30.0, savedly, savedlx, 0.0) end

    Screen.flip()
end