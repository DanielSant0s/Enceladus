

-- dofile("pads/pads.lua")

while true do
    Screen.clear(Color.new(255, 0, 0))

    Graphics.drawTriangle(200, 150, 250, 200, 250, 300, Color.new(64, 0, 128), Color.new(0, 128, 32), Color.new(255, 255, 0))

    Graphics.drawTriangle(0, 0, 0, 100, 100, 100, Color.new(0, 255, 0))

    Graphics.drawRect(400, 300, 100, 100, Color.new(0, 0, 255))

    Graphics.drawPixel(500, 420, Color.new(255, 128, 0))

    Graphics.drawLine(0, 200, 300, 70, Color.new(255, 255, 255));

    Screen.flip()
end