--Screen.setMode(NTSC, 640, 448, CT24, INTERLACED, FIELD, true, Z16S)

Font.ftInit()

local minecraft = Font.ftLoad("minecraft.ttf")

--Screen.setVSync(0)

local models = {Render.loadOBJ("mesh/earth.obj"), Render.loadOBJ("mesh/moon.obj"), Render.loadOBJ("mesh/Car.obj")}
local modIdx = 1

Camera.position(0.0, 0.0,  10.0)
Camera.rotation(0.0, 180.0, 0.0)

local main_light = Lights.new()
Lights.set(main_light, LIGHT_POSITION, 0, 1.0, 1.0, 0)
Lights.set(main_light, LIGHT_DIFFUSE, 1, 1, 1, 0)
Lights.set(main_light, LIGHT_SPECULAR, 0.8, 0.8, 0.8, 0)
--Lights.set(main_light, LIGHT_AMBIENT, 0.2, 0.2, 0.2, 0)

local lx = nil
local ly = nil
local rx = nil
local ry = nil
local pad = nil
local oldpad = nil
local modeltodisplay = 0

local savedlx = 0.0
local savedly = 180.0

local savedrx = 0.0
local savedry = 0.0

local modelz = 0.0

local show_bbox = 0

Screen.getFPS(720)

local timer = Timer.new()

while true do
    oldpad = pad
    pad = Pads.get()

    if Timer.getTime(timer) > 720 then
        print("FPS: " .. Screen.getFPS(720))
        Timer.reset(timer)
    end

    lx, ly = Pads.getLeftStick()
    lx = lx / 512.0
    ly = ly / 512.0
    savedlx = savedlx - lx
    savedly = savedly - ly

    rx, ry = Pads.getRightStick()
    rx = rx / 512.0
    ry = ry / 512.0
    savedrx = savedrx + rx
    savedry = savedry + ry

    Screen.clear(Color.new(40, 40, 40))

    Graphics.setView(0.0, 640.0, 0.0, 448.0)

    Font.ftPrint(minecraft, 30, 15,  0, 640, 448, "Enceladus Render Demo | Graphics by PS2GL")

    Render.setView(40.0, 4/3, 1.0, 4000.0)

    if Pads.check(pad, PAD_R2) then
       modelz = modelz + 0.5
    end

    if Pads.check(pad, PAD_L2) then
        modelz = modelz - 0.5
    end

    if Pads.check(pad, PAD_RIGHT) and not Pads.check(oldpad, PAD_RIGHT) and modIdx < 3 then
        modIdx = modIdx + 1
    end

    if Pads.check(pad, PAD_LEFT) and not Pads.check(oldpad, PAD_LEFT) and modIdx > 1 then
        modIdx = modIdx - 1
    end

    if Pads.check(pad, PAD_CROSS) and not Pads.check(oldpad, PAD_CROSS) then
        if show_bbox == 0 then
            show_bbox = 1
        else 
            show_bbox = 0
        end
    end

    Render.drawOBJ(models[modIdx], savedrx, savedry, modelz, savedly, savedlx, 0.0)

    if show_bbox == 1 then 
        Render.drawBbox(models[modIdx],  savedrx, savedry, modelz, savedly, savedlx, 0.0, Color.new(0, 255, 0))
    end

    Screen.flip()
end