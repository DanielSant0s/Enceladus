-- change this to select the view area
depth = 32
x0 = -0.65
y0 = -0.7
x1 = -0.5
y1 = -0.6

-- some nice palette
palette = {}
for i=0,depth do
	b = math.floor(i / depth * 1024)
	if b > 255 then
		b = 255
	end
	g = math.floor((i - depth / 3) / depth * 1024)
	if g > 255 then
		g = 255
	end
	if g < 0 then
		g = 0
	end
	r = math.floor((i - depth / 3 * 2) / depth * 1024)
	if r > 255 then
		r = 255
	end
	if r < 0 then
		r = 0
	end
	palette[i] = Color.new(r, g, b)
end
palette[depth-1] = Color.new(0, 0, 0)

-- draw mandelbrot fractal
w = 640
h = 448
dx = x1 - x0
dy = y1 - y0
for y=0,h-1 do
	for x=0,w-1 do
		r = 0; n = 0; b = x / w * dx + x0; e = y / h * dy + y0; i = 0
		while i < depth-1 and r * r < 4 do
			d = r; r = r * r - n * n + b; n = 2 * d * n + e; i = i + 1
		end
		Graphics.drawPixel(x, y, palette[i])
	end
	Screen.waitVblankStart()
	Screen.flip()
end

while true do
    Screen.waitVblankStart()
end
