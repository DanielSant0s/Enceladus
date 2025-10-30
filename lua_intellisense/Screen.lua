---@meta
---@file intellisense metadata corresponding to the `Color` and `Screen` libraries inside `src/lua/screen.cpp`
---@diagnostic disable


---@class Color
Color = {}
---@class Screen
Screen = {}

--- Creates a Color
---@param R integer RED color **[0-255]**
---@param G integer BLUE color **[0-255]**
---@param B integer GREEN color **[0-255]**
---@param A integer ALPHA Channel **[0-128]**. optional parameter, if not passed, 128 is assumed (no transparency)
---@return color color generated color, to be used as color parameter on the `Screen` and `Graphics` functions
---@overload fun(R:integer, G:integer, B:integer): color:color
function Color.new(R, G, B, A) end

function Color.getR(Color) end
function Color.getG(Color) end
function Color.getB(Color) end
function Color.getA(Color) end

---@type VideoMode
NTSC=2;
---@type VideoMode
_480p= 0x50;
---@type VideoMode
PAL=3;
---@type VideoMode
_576p=0x53;
---@type VideoMode
_720p=0x52;
---@type VideoMode
_1080i=0x51;

---@type Interlace
INTERLACED=1;
---@type Interlace
NONINTERLACED=0;

---@type Field
FIELD=0;
---@type Field
FRAME=1;

---@type ColorMode
CT32=0;
---@type ColorMode
CT24=1;
---@type ColorMode
CT16=2;
---@type ColorMode
CT16S=0xA;

---@type ColorModeZbuffer
Z32=0;
---@type ColorModeZbuffer
Z24=1;
---@type ColorModeZbuffer
Z16=2;
---@type ColorModeZbuffer
Z16S=0x0A;

--- Changes the Screen mode, the last two params are optional, **dont use them unless interested in 3D**
--- @param mode VideoMode video mode to be used, if not sure, chose PAL or NTSC depending on console region
---@param width integer  width  of the image buffer
---@param height integer height of the image buffer
---@param colormode ColorMode
---@param interlace Interlace
---@param field Field
---@param zbuffering boolean
---@param zbuf_colormode ColorModeZbuffer
---@see VideoMode
---@see ColorMode
---@see Interlace
---@see Field
---@see ColorModeZbuffer
function Screen.setMode(mode, width, height, colormode, interlace, field, zbuffering, zbuf_colormode) end

--- returns a lua table containing the current video configuration
--- 
--- the table contains the following elements, all of them integer
--- - mode
--- - interlace
--- - field
--- - dithering
--- - doubleBuffering
--- - zBuffering
--- - width
--- - height
--- - aspect
--- - colorMode
--- - zColorMode
--- @return table MODE
function Screen.getMode() end

--- Clears the screen
--- @param color color **[optional]**: if no parameter is passed, black is assumed
--- @overload fun()
function Screen.clear(color) end

--- Flip Screen
--- 
function Screen.flip() end

--- returns the ammount of free ram in bytes
--- @return integer freeVRAM
function Screen.getFreeVRAM() end

function Screen.waitVblankStart() end

---@param c integer
function Screen.getFPS(c) end

---@deprecated
---@see Screen.getFPS
function System.getFPS(prev, cur) end
