Font.ftInit()
local grogu = Graphics.loadImage("System/images/grogu.jpg")
local stars = Graphics.loadImage("System/images/stars.bmp")
local ink = Graphics.loadImage("System/images/ink.png")

Color1 = Color.new(0, 255, 255, 255)
Color2 = Color.new(255, 0, 255, 255)
Color3 = Color.new(255, 255, 0, 255)
Color4 = Color.new(0, 255, 0, 255)

Font.fmLoad()
local netron = Font.ftLoad("System/netron.ttf")
Font.ftSetPixelSize(netron, 25, 25)

io.write("\nFont Handle: " .. netron .. "\n")

local type, freespace, format = System.getMCInfo(0)

function printf(...)
    io.write(string.format(...))
   end
   

printf("Hello %s from %s on %s\n",os.getenv"USER" or "there",_VERSION,os.date())

while true do

    Screen.clear()

    local pad = Pads.get()

    if Pads.check(pad, PAD_START) then
        System.exitToBrowser()
    end

    if Pads.check(pad, PAD_SELECT) then
        Screen.setMode(PAL, 640, 512, CT16S, INTERLACED, FIELD)
    end

    Graphics.drawImage(grogu, 0, 0)
    Graphics.drawImage(ink, 0, -50)
    Graphics.drawImage(stars, 100, 150, 255, 600, 600)
    Graphics.drawLine(200, 200, 100, 100, Color.new(255, 255, 255, 255));
    Graphics.drawPixel(180, 140, Color.new(0, 0, 255, 255))
    Graphics.drawTriangle(605.0, 442.0, 522.0, 315.0, 400.0, 350.0, Color.new(220, 40, 40, 255))
    Graphics.drawTriangle(300.0, 200.0, 300.0, 350.0, 400.0, 350.0, Color1, Color2, Color3)
    Graphics.drawRect(220.0, 280.0, 75, 75, Color.new(128, 128, 128, 32))
    Graphics.drawQuad(500.0, 250.0, 500.0, 350.0, 600.0, 250.0, 600.0, 350.0, Color1, Color2, Color1, Color2)
    Graphics.drawCircle(430.0, 220.0, 80.0, Color.new(255, 0, 0, 255))
    Font.fmPrint(200, 60, 0.6, "MC Info\nType: " .. type .. "\nFree Space: " .. freespace .. "\nFormat: " .. format .. "\n")
    Font.ftPrint(netron, 60, 60, 0, 200, 200, "MC Info\nType: " .. type .. "\nFree Space: " .. freespace .. "\nFormat: " .. format .. "\n")
    Screen.waitVblankStart()
    Screen.flip()
end