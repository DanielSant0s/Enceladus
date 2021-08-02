green = Color.new(0, 255, 0)

Font.fmLoad()


while true do
Screen.clear()
Font.fmPrint(250, 200, 0.6, "Hello World!", green)

for i=0,20 do
	x0 = i/20*639
	y1 = 271-i/20*481
	Graphics.drawLine(x0, 481, 639, y1, green)
end
	Screen.flip()
	Screen.waitVblankStart()
end
