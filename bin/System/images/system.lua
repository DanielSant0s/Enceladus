
--Screen.showSplash()
Font.ftInit()
Render.init()

local grogu = Graphics.loadImage("System/images/grogu.jpg")
local stars = Graphics.loadImage("System/images/stars.bmp")
local ink = Graphics.loadImage("System/images/ink.png")
local mario = Graphics.loadImage("System/images/mario.png")

Color1 = Color.new(0, 255, 255, 255)
Color2 = Color.new(255, 0, 255, 255)
Color3 = Color.new(255, 255, 0, 255)
Color4 = Color.new(0, 255, 0, 255)

local rad = 0
local mx = 100.0

Font.fmLoad()
local netron = Font.ftLoad("System/images/netron.ttf")
Font.ftSetPixelSize(netron, 25, 25)

local type, freespace, format = System.getMCInfo(0)

local rx, ry
romver = System.openFile("rom0:ROMVER", FREAD)
romver_size = System.sizeFile(romver)
romver_data = System.readFile(romver, romver_size)
System.closeFile(romver)

--local cube = Render.load("System/images/mesh/cube.obj")
local bomdecoito = Render.load("System/images/mesh/crashbandicoot.obj")

Camera.position(0, 0, 50)
Camera.rotation(0, 0, 0)

Lights.create(4)

Lights.set(0, 0.0, 0.0, 0.0, 0, 0, 0, AMBIENT)
Lights.set(1, 1.0, 0.0, -1.0, 0.6, 1, 0.6, DIRECTIONAL)
Lights.set(2, 0.0, 1.0, -1.0, 0.3, 0.3, 0.3, DIRECTIONAL)
Lights.set(3, -1.0, -1.0, -1.0, 0.5, 0.5, 0.5, DIRECTIONAL)

while true do

    Screen.clear(Color.new(0, 0, 0, 255))

    local pad = Pads.get()

    if Pads.check(pad, PAD_START) then
        System.exitToBrowser()
    end

    if Pads.check(pad, PAD_SELECT) then
        Screen.setMode(_480p, 640, 480, CT16S, NONINTERLACED, FRAME)
    end

    Graphics.drawImage(grogu, 320, 224)
    Graphics.drawScaleImage(stars, 100, 150, 400, 400)
    Graphics.drawImageExtended(ink, 500, 450, 128, 0, 256, 512, 128, 128, 0, Color.new(128, 128, 0, 128))
    Graphics.drawLine(200, 200, 100, 100, Color.new(255, 255, 255, 255));
    Graphics.drawPixel(180, 140, Color.new(0, 0, 255, 255))
    Graphics.drawTriangle(605.0, 442.0, 522.0, 315.0, 400.0, 350.0, Color.new(220, 40, 40, 255))
    Graphics.drawTriangle(300.0, 200.0, 300.0, 350.0, 400.0, 350.0, Color1, Color2, Color3)
    Graphics.drawRect(220.0, 280.0, 75, 75, Color.new(128, 128, 128, 32))
    Graphics.drawQuad(500.0, 250.0, 500.0, 350.0, 600.0, 250.0, 600.0, 350.0, Color1, Color2, Color1, Color2)
    Graphics.drawCircle(430.0, 220.0, 80.0, Color.new(255, 0, 0, 255))
    Graphics.drawRotateImage(mario, 400, 224, rad)
    rad = rad + 0.05
    Graphics.drawImage(mario, mx, 224, Color.new(0, 128, 128, 128))
    mx = mx + 0.7

    Font.fmPrint(250, 350, 0.6, "ROMVER: " .. romver_data .. "\n")
    Font.fmPrint(200, 60, 0.6, "MC Info\nType: " .. type .. "\nFree Space: " .. freespace .. "\nFormat: " .. format .. "\n")
    Font.ftPrint(netron, 60, 60, 0, 200, 200, "MC Info\nType: " .. type .. "\nFree Space: " .. freespace .. "\nFormat: " .. format .. "\n")
    
    --Render.draw(cube, 0.0, 2.0, 20.0, rad, rad, 0.0)
    Render.draw(bomdecoito, 5.0, 2.0, -1000.0, rad, rad, 0.0)
    
    --Screen.waitVblankStart()
    Screen.flip()
end