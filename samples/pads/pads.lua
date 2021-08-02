--Screen.showSplash()

Font.fmLoad()

local circle = Graphics.loadImage("samples/pads/circle.png")
local cross = Graphics.loadImage("samples/pads/cross.png")
local square = Graphics.loadImage("samples/pads/square.png")
local triangle = Graphics.loadImage("samples/pads/triangle.png")

local up = Graphics.loadImage("samples/pads/up.png")
local down = Graphics.loadImage("samples/pads/down.png")
local left = Graphics.loadImage("samples/pads/left.png")
local right = Graphics.loadImage("samples/pads/right.png")

local start = Graphics.loadImage("samples/pads/start.png")
local pad_select = Graphics.loadImage("samples/pads/select.png")

local r1 = Graphics.loadImage("samples/pads/r1.png")
local r2 = Graphics.loadImage("samples/pads/r2.png")

local l1 = Graphics.loadImage("samples/pads/l1.png")
local l2 = Graphics.loadImage("samples/pads/l2.png")

local l3 = Graphics.loadImage("samples/pads/l3.png")
local r3 = Graphics.loadImage("samples/pads/r3.png")

local pad = nil
local rx = nil
local ry = nil
local lx = nil
local ly = nil

while true do
  Screen.clear()

  Font.fmPrint(150, 25, 0.6, "\nEnceladus project: Controls demo\n")
  Font.fmPrint(100, 370, 0.4, "\nTips:\n")
  Font.fmPrint(100, 390, 0.4, "\nPress R2+L2 to start rumble and R3+L3 to stop it.\n")
  Font.fmPrint(100, 405, 0.4, "\nButtons transparency varies with the pressure applied to them\n")

  pad = Pads.get()
  rx, ry = Pads.getRightStick()
  lx, ly = Pads.getLeftStick()

  if Pads.check(pad, PAD_SELECT) then
    Graphics.drawImage(pad_select, 260.0, 190.0)
  else
    Graphics.drawImage(pad_select, 260.0, 190.0, 60)
  end
  
  if Pads.check(pad, PAD_START) then
    Graphics.drawImage(start, 380.0, 190.0)
  else
    Graphics.drawImage(start, 380.0, 190.0, 60)
  end

  if Pads.check(pad, PAD_UP) then
    local pressure = Pads.getPressure(PAD_UP)
    Graphics.drawImage(up, 120.0, 155.0, pressure)
  else
    Graphics.drawImage(up, 120.0, 155.0, 60)
  end

  if Pads.check(pad, PAD_DOWN) then
    local pressure = Pads.getPressure(PAD_DOWN)
    Graphics.drawImage(down, 120.0, 225.0, pressure)
  else
    Graphics.drawImage(down, 120.0, 225.0, 60)
  end

  if Pads.check(pad, PAD_LEFT) then
    local pressure = Pads.getPressure(PAD_LEFT)
    Graphics.drawImage(left, 85.0, 190.0, pressure)
  else
    Graphics.drawImage(left, 85.0, 190.0, 60)
  end

  if Pads.check(pad, PAD_RIGHT) then
    local pressure = Pads.getPressure(PAD_RIGHT)
    Graphics.drawImage(right, 155.0, 190.0, pressure)
  else
    Graphics.drawImage(right, 155.0, 190.0, 60)
  end

  if Pads.check(pad, PAD_TRIANGLE) then
    local pressure = Pads.getPressure(PAD_TRIANGLE)
    Graphics.drawImage(triangle, 520.0, 155.0, pressure)
  else
    Graphics.drawImage(triangle, 520.0, 155.0, 60)
  end

  if Pads.check(pad, PAD_CROSS) then
    local pressure = Pads.getPressure(PAD_CROSS)
    Graphics.drawImage(cross, 520.0, 225.0, pressure)
  else
    Graphics.drawImage(cross, 520.0, 225.0, 60)
  end

  if Pads.check(pad, PAD_SQUARE) then
    local pressure = Pads.getPressure(PAD_SQUARE)
    Graphics.drawImage(square, 485.0, 190.0, pressure)
  else
    Graphics.drawImage(square, 485.0, 190.0, 60)
  end

  if Pads.check(pad, PAD_CIRCLE) then
    local pressure = Pads.getPressure(PAD_CIRCLE)
    Graphics.drawImage(circle, 555.0, 190.0, pressure)
  else
    Graphics.drawImage(circle, 555.0, 190.0, 60)
  end

  if Pads.check(pad, PAD_L1) then
    local pressure = Pads.getPressure(PAD_L1)
    Graphics.drawImage(l1, 102.0, 100.0, pressure)
  else
    Graphics.drawImage(l1, 102.0, 100.0, 60)
  end

  if Pads.check(pad, PAD_L2) then
    local pressure = Pads.getPressure(PAD_L2)
    Graphics.drawImage(l2, 137.0, 100.0, pressure)
  else
    Graphics.drawImage(l2, 137.0, 100.0, 60)
  end

  if Pads.check(pad, PAD_R1) then
    local pressure = Pads.getPressure(PAD_R1)
    Graphics.drawImage(r1, 502.0, 100.0, pressure)
  else
    Graphics.drawImage(r1, 502.0, 100.0, 60)
  end

  if Pads.check(pad, PAD_R2) then
    local pressure = Pads.getPressure(PAD_R2)
    Graphics.drawImage(r2, 537.0, 100.0, pressure)
  else
    Graphics.drawImage(r2, 537.0, 100.0, 60)
  end

  Graphics.drawCircle(lx/4+257.0, ly/4+317.0, 8.0, Color.new(255, 0, 0, 255), 1)
  Graphics.drawCircle(rx/4+417.0, ry/4+317.0, 8.0, Color.new(255, 0, 0, 255), 1)

  Graphics.drawRect(220.0, 280.0, 75, 75, Color.new(128, 128, 128, 32))
  Graphics.drawRect(380.0, 280.0, 75, 75, Color.new(128, 128, 128, 32))

  if Pads.check(pad, PAD_L3) then
    Graphics.drawImage(l3, 242.0, 300.0)
  else
    Graphics.drawImage(l3, 242.0, 300.0, 60)
  end

  if Pads.check(pad, PAD_R3) then
    Graphics.drawImage(r3, 402.0, 300.0)
  else
    Graphics.drawImage(r3, 402.0, 300.0, 60)
  end

  if Pads.check(pad, PAD_R2) and Pads.check(pad, PAD_L2) then
    Pads.rumble(1 ,255)
  end

  if Pads.check(pad, PAD_R3) and Pads.check(pad, PAD_L3) then
    Pads.rumble(0 ,0)
  end
  
  Screen.flip()
  Screen.waitVblankStart()
end
