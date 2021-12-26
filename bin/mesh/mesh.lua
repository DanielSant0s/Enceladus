Screen.setMode(NTSC, 640, 448, CT24, INTERLACED, FIELD, true, Z16S)

Render.init(4/3)

local orangetex = Graphics.loadImage("mesh/moon.jpg")
Graphics.setImageFilters(orangetex, LINEAR) 
local orange = Render.loadOBJ("mesh/moon.obj", orangetex)

Camera.position(0.0, 0.0, 50.0)
Camera.rotation(0.0, 0.0,  0.0)

Lights.create(4)

--Lights.set(1,  0.0,  0.0,  0.0, 1.0, 1.0, 1.0,     AMBIENT)
Lights.set(2,  1.0,  0.0, -1.0, 1.0, 1.0, 1.0, DIRECTIONAL)
--Lights.set(3,  0.0,  1.0, -1.0, 0.9, 0.5, 0.5, DIRECTIONAL)
--Lights.set(4, -1.0, -1.0, -1.0, 0.5, 0.5, 0.5, DIRECTIONAL)

local lx = nil
local ly = nil
local rx = nil
local yy = nil
local pad = nil
local oldpad = nil
local modeltodisplay = 0

local savedlx = 0.0
local savedly = 180.0

local savedrx = 0.0
local savedry = 0.0

local modelz = 0.0

while true do

    oldpad = pad
    pad = Pads.get()

    lx, ly = Pads.getLeftStick()
    lx = lx / 1024.0
    ly = ly / 1024.0
    savedlx = savedlx - lx
    savedly = savedly - ly

    rx, ry = Pads.getRightStick()
    rx = rx / 1024.0
    ry = ry / 1024.0
    savedrx = savedrx + rx
    savedry = savedry + ry

    Screen.clear(Color.new(40, 40, 40, 128))

    if Pads.check(pad, PAD_R2) then
       modelz = modelz + 0.5
    end

    if Pads.check(pad, PAD_L2) then
        modelz = modelz - 0.5
     end

    Render.drawOBJ(orange,  savedrx, savedry, modelz, savedly, savedlx, 0.0)

    Screen.flip()
end