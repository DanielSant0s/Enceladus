Font.ftInit()

--Screen.setVSync(0);

local minecraft = Font.ftLoad("minecraft.ttf")

local circle = Graphics.loadImage("pads/circle.png")
local cross = Graphics.loadImage("pads/cross.png")
local square = Graphics.loadImage("pads/square.png")
local triangle = Graphics.loadImage("pads/triangle.png")

local up = Graphics.loadImage("pads/up.png")
local down = Graphics.loadImage("pads/down.png")
local left = Graphics.loadImage("pads/left.png")
local right = Graphics.loadImage("pads/right.png")

local start = Graphics.loadImage("pads/start.png")
local pad_select = Graphics.loadImage("pads/select.png")

local r1 = Graphics.loadImage("pads/R1.png")
local r2 = Graphics.loadImage("pads/R2.png")

local l1 = Graphics.loadImage("pads/L1.png")
local l2 = Graphics.loadImage("pads/L2.png")

local l3 = Graphics.loadImage("pads/L3.png")
local r3 = Graphics.loadImage("pads/R3.png")

local pad = nil
local rx = nil
local ry = nil
local lx = nil
local ly = nil
local pressure = nil

local trans_white = Color.new(255, 255, 255, 60);
local trans_mwhite = Color.new(255, 255, 255, 32);
local opaque = Color.new(255, 255, 255, 255);

while true do
  Screen.clear()

  Font.ftPrint(minecraft, 0, 0,  0, 0, 0, Screen.getFPS(360) .. " FPS")

  Font.ftPrint(minecraft, 150, 25,  0, 640, 448, "\nEnceladus project: Controls demo\n")
  Font.ftPrint(minecraft, 100, 370, 0, 640, 448, "\nTips:\n")
  Font.ftPrint(minecraft, 100, 390, 0, 640, 448, "\nPress R2+L2 to start rumble and R3+L3 to stop it.\n")
  Font.ftPrint(minecraft, 100, 405, 0, 640, 448, "\nButtons transparency varies with the pressure applied to them\n")

  pad = Pads.get()
  rx, ry = Pads.getRightStick()
  lx, ly = Pads.getLeftStick()

    if Pads.check(pad, PAD_SELECT) then
      Graphics.drawImage(pad_select, 260.0, 190.0)
    else
      Graphics.drawImage(pad_select, 260.0, 190.0, trans_white)
    end

    if Pads.check(pad, PAD_START) then
      Graphics.drawImage(start, 380.0, 190.0)
    else
      Graphics.drawImage(start, 380.0, 190.0, trans_white)
    end

    if Pads.check(pad, PAD_UP) then
      pressure = Pads.getPressure(PAD_UP)
      Graphics.drawImage(up, 120.0, 155.0, Color.new(255, 255, 255, pressure))
    else
      Graphics.drawImage(up, 120.0, 155.0, trans_white)
    end

    if Pads.check(pad, PAD_DOWN) then
      pressure = Pads.getPressure(PAD_DOWN)
      Graphics.drawImage(down, 120.0, 225.0, Color.new(255, 255, 255, pressure))
    else
      Graphics.drawImage(down, 120.0, 225.0, trans_white)
    end

    if Pads.check(pad, PAD_LEFT) then
      pressure = Pads.getPressure(PAD_LEFT)
      Graphics.drawImage(left, 85.0, 190.0, Color.new(255, 255, 255, pressure))
    else
      Graphics.drawImage(left, 85.0, 190.0, trans_white)
    end

    if Pads.check(pad, PAD_RIGHT) then
      pressure = Pads.getPressure(PAD_RIGHT)
      Graphics.drawImage(right, 155.0, 190.0, Color.new(255, 255, 255, pressure))
    else
      Graphics.drawImage(right, 155.0, 190.0, trans_white)
    end

    if Pads.check(pad, PAD_TRIANGLE) then
      pressure = Pads.getPressure(PAD_TRIANGLE)
      Graphics.drawImage(triangle, 520.0, 155.0, Color.new(255, 255, 255, pressure))
    else
      Graphics.drawImage(triangle, 520.0, 155.0, trans_white)
    end

   if Pads.check(pad, PAD_CROSS) then
     pressure = Pads.getPressure(PAD_CROSS)
     Graphics.drawImage(cross, 520.0, 225.0, Color.new(255, 255, 255, pressure))
   else
     Graphics.drawImage(cross, 520.0, 225.0, trans_white)
   end

   if Pads.check(pad, PAD_SQUARE) then
     pressure = Pads.getPressure(PAD_SQUARE)
     Graphics.drawImage(square, 485.0, 190.0, Color.new(255, 255, 255, pressure))
   else
     Graphics.drawImage(square, 485.0, 190.0, trans_white)
   end

   if Pads.check(pad, PAD_CIRCLE) then
     pressure = Pads.getPressure(PAD_CIRCLE)
     Graphics.drawImage(circle, 555.0, 190.0, Color.new(255, 255, 255,  pressure))
   else
     Graphics.drawImage(circle, 555.0, 190.0, trans_white)
   end

  if Pads.check(pad, PAD_L1) then
    pressure = Pads.getPressure(PAD_L1)
    Graphics.drawImage(l1, 102.0, 100.0, Color.new(255, 255, 255, pressure))
  else
    Graphics.drawImage(l1, 102.0, 100.0, trans_white)
  end

  if Pads.check(pad, PAD_L2) then
    pressure = Pads.getPressure(PAD_L2)
    Graphics.drawImage(l2, 137.0, 100.0, Color.new(255, 255, 255, pressure))
  else
    Graphics.drawImage(l2, 137.0, 100.0, trans_white)
  end

  if Pads.check(pad, PAD_R1) then
    pressure = Pads.getPressure(PAD_R1)
    Graphics.drawImage(r1, 502.0, 100.0, Color.new(255, 255, 255, pressure))
  else
    Graphics.drawImage(r1, 502.0, 100.0, trans_white)
  end

  if Pads.check(pad, PAD_R2) then
    pressure = Pads.getPressure(PAD_R2)
    Graphics.drawImage(r2, 537.0, 100.0, Color.new(255, 255, 255, pressure))
  else
    Graphics.drawImage(r2, 537.0, 100.0, trans_white)
  end

  Graphics.drawRect(220.0, 280.0, 75, 75, trans_mwhite)
  Graphics.drawRect(380.0, 280.0, 75, 75, trans_mwhite)

  if Pads.check(pad, PAD_L3) then
    Graphics.drawImage(l3, lx/4.0+242.0, ly/4.0+300.0, opaque)
  else 
    Graphics.drawImage(l3, lx/4.0+242.0, ly/4.0+300.0, trans_white)
  end

  if Pads.check(pad, PAD_R3) then
    Graphics.drawImage(r3, rx/4.0+402.0, ry/4.0+300.0, opaque)
  else 
    Graphics.drawImage(r3, rx/4.0+402.0, ry/4.0+300.0, trans_white)
  end

  if Pads.check(pad, PAD_R2) and Pads.check(pad, PAD_L2) then
    Pads.rumble(1 ,255)
  end

  if Pads.check(pad, PAD_R3) and Pads.check(pad, PAD_L3) then
    Pads.rumble(0 ,0)
  end
  
  Screen.flip()
end