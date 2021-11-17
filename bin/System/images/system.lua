
--Screen.showSplash()
Font.ftInit()

local grogu = Graphics.loadImage("System/images/grogu.jpg")
local stars = Graphics.loadImage("System/images/stars.bmp")
local ink = Graphics.loadImage("System/images/ink.png")
local mario = Graphics.loadImage("System/images/mario.png")

Color1 = Color.new(0, 255, 255, 255)
Color2 = Color.new(255, 0, 255, 255)
Color3 = Color.new(255, 255, 0, 255)
Color4 = Color.new(0, 255, 0, 255)

local rad = 0

Font.fmLoad()
local netron = Font.ftLoad("System/images/netron.ttf")
Font.ftSetPixelSize(netron, 25, 25)

local type, freespace, format = System.getMCInfo(0)

local mx, my
romver = System.openFile("rom0:ROMVER", FREAD)
romver_size = System.sizeFile(romver)
romver_data = System.readFile(romver, romver_size)
System.closeFile(romver)


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
    --Graphics.drawRotateImage(mario, 400, 224, rad)
    rad = rad + 0.005
    --Graphics.drawImage(mario, mx, 224, Color.new(0, 128, 128, 128))

    Font.fmPrint(250, 350, 0.6, "ROMVER: " .. romver_data .. "\n")
    Font.fmPrint(200, 60, 0.6, "MC Info\nType: " .. type .. "\nFree Space: " .. freespace .. "\nFormat: " .. format .. "\n")
    Font.ftPrint(netron, 60, 60, 0, 200, 200, "MC Info\nType: " .. type .. "\nFree Space: " .. freespace .. "\nFormat: " .. format .. "\n")
    
    --Screen.waitVblankStart()
    Screen.flip()
end