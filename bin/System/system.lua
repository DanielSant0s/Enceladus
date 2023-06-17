

-- dofile("pads/pads.lua")

local vw = Graphics.loadImage("vw.png");
local vw8bit = Graphics.loadImage("vw8bit.png");
local vw4bit = Graphics.loadImage("vw4bit.png");

while true do
    Screen.clear(Color.new(32, 32, 32))

    Graphics.drawTriangle(200, 150, 250, 200, 250, 300, Color.new(64, 0, 128), Color.new(0, 128, 32), Color.new(255, 255, 0))

    Graphics.drawTriangle(0, 0, 0, 100, 100, 100, Color.new(0, 255, 0))

    Graphics.drawRect(400, 300, 100, 100, Color.new(0, 0, 255))

    Graphics.drawPixel(500, 420, Color.new(255, 128, 0))

    Graphics.drawLine(0, 200, 300, 70, Color.new(255, 255, 255));

    Graphics.drawQuad(70, 70, 90, 85, 70, 105, 90, 120, Color.new(0, 0, 128))

    Graphics.drawQuad(110, 70, 180, 85, 60, 130, 42, 215, Color.new(255, 0, 0), Color.new(64, 0, 128), Color.new(0, 128, 32), Color.new(255, 255, 0))

    Graphics.drawCircle(400, 29, 130, Color.new(255, 255, 255))

    Graphics.drawCircle(400, 29, 50, Color.new(255, 0, 0), 0)

    Graphics.drawScaleImage(vw, 0, 128, 128, 128, Color.new(255, 255, 255));
    Graphics.drawScaleImage(vw8bit, 0, 256, 128, 128, Color.new(255, 255, 255));
    Graphics.drawScaleImage(vw4bit, 128, 256, 128, 128, Color.new(255, 255, 255));

    Screen.flip()
end