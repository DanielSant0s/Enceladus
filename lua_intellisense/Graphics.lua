--- @meta
--- @file intellisense metadata corresponding to the `Graphics` and `Font` library inside `src/lua/graphics.cpp`
--- @diagnostic disable

--#region Graphics

---@class Graphics
Graphics = {}

---@class Font
Font = {}

--- @alias color integer Color value, can be generated with Color.new()
--- @see Color.new

--- Changes the color of a specific pixel
--- @param X integer X coordinate
--- @param Y integer Y coordinate
--- @param color color the color of the pixel
--- @see color
function Graphics.drawPixel(X, Y, color) end

--- @param X integer X coordinate for top left corner of the rectangle
--- @param Y integer Y coordinate for top left corner of the rectangle
--- @param width integer rectangle width
--- @param height integer rectangle height
--- @param color color the color of the rectangle
function Graphics.drawRect(X, Y, width, height, color) end

--- @param X integer X coordinate for begining of line
--- @param Y integer Y coordinate for begining of line
--- @param X2 integer X coordinate for end of the line
--- @param Y2 integer Y coordinate for end of the line
--- @param color color the color of the line
function Graphics.drawLine(X, Y, X2, Y2, color) end

--- @param X integer X coordinate for circle center
--- @param Y integer Y coordinate for circle center
--- @param radius integer circle radius
--- @param color color circle color
--- @param fill integer **1:**draw a filled circle, **0:**draw only the perimeter of the circle. _Optional parameter: 0 is the default_
--- @overload fun(X:integer, Y:integer, radius:integer, color:color)
function Graphics.drawCircle(X, Y, radius, color, fill) end

--- @param x  number Coordinates for first  vertice
--- @param y  number Coordinates for first  vertice
--- @param x2 number Coordinates for second vertice
--- @param y2 number Coordinates for second vertice
--- @param x3 number Coordinates for third  vertice
--- @param y3 number Coordinates for third  vertice
--- @param color color  
--- @overload fun(x:number, y:number, x2:number, y2:number, x3:number, y3:number)
function Graphics.drawTriangle(x, y, x2, y2, x3, y3, color) end

---@param x  number coordinates for first  corner
---@param y  number coordinates for first  corner
---@param x2 number coordinates for second corner
---@param y2 number coordinates for second corner
---@param x3 number coordinates for third  corner
---@param y3 number coordinates for third  corner
---@param x4 number coordinates for fourth corner
---@param y4 number coordinates for fourth corner
---@param color color
function Graphics.drawQuad(x, y, x2, y2, x3, y3, x4, y4, color) end

---@param x  number coordinates for first  corner
---@param y  number coordinates for first  corner
---@param x2 number coordinates for second corner
---@param y2 number coordinates for second corner
---@param x3 number coordinates for third  corner
---@param y3 number coordinates for third  corner
---@param x4 number coordinates for fourth corner
---@param y4 number coordinates for fourth corner
---@param color color 
---@param color2 color 
---@param color3 color 
---@param color4 color 
function Graphics.drawQuad(x, y, x2, y2, x3, y3, x4, y4, color, color2, color3, color4) end

--- @alias Texture integer

--- Loads a PNG/JPG/BMP image from a file
--- @param path string location of the image to load
--- @param delayed boolean Whether the texture manager will be used or not. **Optional Param:** true by default
--- @return Texture|nil texture
--- @overload fun(path:string):Texture|nil:texture
--- @nodiscard
function Graphics.loadImage(path, delayed) end

--- Threaded version of Graphics.loadImage
--- @param path string location of the image to load
--- @param delayed boolean Whether the texture manager will be used or not. **Optional Param:** true by default
--- @see Graphics.loadImage
function Graphics.threadLoadImage(path, delayed) end

--- @return integer stat status of the threaded image load
--- return values can be:  
--- - `-2`: no thread running, no texture avaiable for pickup
--- - `-1`: error creating thread
--- - ` 0`: thread running
--- - ` 1`: thread finished, texture avaiable for pickup with `getLoadData`
--- - ` 2`: thread finished, failed to load texture  
--- @see Graphics.threadLoadImage
--- @see Graphics.getLoadData
--- @nodiscard
function Graphics.getLoadState() end

--- @return Texture|nil texture texture loaded by threadLoadImage. returns nil if there is no instance of threadLoadImage running
--- @see Graphics.threadLoadImage
--- @see Graphics.getLoadState
--- @nodiscard
function Graphics.getLoadData() end

--- Draws an image on screen with it's original size
--- @param image Texture The image texture
--- @param X integer the X coordinates for the top left corner of the image
--- @param Y integer the Y coordinates for the top left corner of the image
--- @param color color **[Optional]**: if not specified default is RGBA #80808080
--- @overload fun(image:Texture, X:integer, Y:integer)
function Graphics.drawImage(image, X, Y, color) end

--- Draws an image on screen with it's original size and a rotation angle
--- @param image Texture The image texture
--- @param X integer the X coordinates for the top left corner of the image
--- @param Y integer the Y coordinates for the top left corner of the image
--- @param angle integer Angle of rotation
--- @param color color A color created by Color.new. [Optional Parameter]
function Graphics.drawRotateImage(image, X, Y, angle, color) end

--- Draws an image with a specified width and height
--- @param image Texture The image texture
--- @param X integer the X coordinates for the top left corner of the image
--- @param Y integer the Y coordinates for the top left corner of the image
--- @param scale_x integer width of the draw space for the image
--- @param scale_y integer height of the draw space for the image
--- @param color color A color created by Color.new. [Optional Parameter]
function Graphics.drawScaleImage(image, X, Y, scale_x, scale_y, color) end

--- Draws a portion of an image
--- @param image Texture The image texture
--- @param X integer the X coordinates for the top left corner of the image
--- @param Y integer the Y coordinates for the top left corner of the image
--- @param start_x integer coordinates for the portion of the image you want to display (top left corner)
--- @param start_y integer coordinates for the portion of the image you want to display (top left corner)
--- @param width integer  width  of the desired portion to be drawn
--- @param height integer height of the desired portion to be drawn
--- @param color color A color created by Color.new. [Optional Parameter]
function Graphics.drawPartialImage(image, X, Y, start_x, start_y, width, height, color) end

--- extended image drawing
--- @param image Texture The image texture
--- @param X integer the X coordinates for the top left corner of the image
--- @param Y integer the Y coordinates for the top left corner of the image
--- @param start_x integer coordinates for the portion of the image you want to display (top left corner)
--- @param start_y integer coordinates for the portion of the image you want to display (top left corner)
--- @param width integer  width  of the desired portion to be drawn
--- @param height integer height of the desired portion to be drawn
--- @param scale_x integer width of the draw space for the image
--- @param scale_y integer height of the draw space for the image
--- @param angle integer Angle of rotation
--- @param color color A color created by Color.new. [Optional Parameter]
function Graphics.drawImageExtended(image, X, Y, start_x, start_y, width, height, scale_x, scale_y, angle, color) end

---@class Filter
--- Filtering macros `NEAREST` or `LINEAR`

---@enum Filter
---@field NEAREST Filter
NEAREST = 0;
---@enum Filter
---@field LINEAR Filter
LINEAR = 1;

--- Changes the filtering mode of the specified texture
--- @param image Texture The image texture
--- @param filter Filter wich filter mode to use. `NEAREST` is the default value upon loading a texture
--- @see Filter
function Graphics.setImageFilters(image, filter) end

---Obtains the width of this texture
--- @param image Texture The image texture
--- @return integer width
function Graphics.getImageWidth(image) end

---Obtains the Height of this texture
--- @param image Texture The image texture
--- @return integer Height
function Graphics.getImageHeight(image) end

--- destroys the texture and frees the allocated buffer where it was stored
--- @param image Texture The image texture
function Graphics.freeImage(image) end

--#endregion

--#region Font

--- Initializes the TTF Fonts system
function Font.ftInit() end

---A numeric ID for a TTF font loaded by [Font.ftLoad](lua://Font.ftLoad)
---@alias ttf integer
---@see Font.ftLoad
---@see Font.ftEnd
---@see Font.ftSetPixelSize
---@see Font.ftSetCharSize
---@see
---@see
---@see Font.ftUnload
---@see Font.ftPrint

--- Loads a TTF font from a file
--- @param path string path to file
--- @return ttf fonthandle ttf font internal ID. see [ttf](lua://ttf)
--- @nodiscard
function Font.ftLoad(path) end

--- Changes font pixel size
--- @param font ttf font handle
--- @param width integer 
--- @param height integer
function Font.ftSetPixelSize(font, width, height) end

--- Changes font char size
--- @param font ttf font handle
--- @param width integer 
--- @param height integer
function Font.ftSetCharSize(font, width, height) end

---@type fontalign
ALIGN_TOP = (0 << 0);
---@type fontalign
ALIGN_BOTTOM = (1 << 0);
---@type fontalign
ALIGN_VCENTER = (2 << 0);
---@type fontalign
ALIGN_LEFT = (0 << 2);
---@type fontalign
ALIGN_RIGHT = (1 << 2);
---@type fontalign
ALIGN_HCENTER = (2 << 2);
---@type fontalign
ALIGN_NONE = (ALIGN_TOP | ALIGN_LEFT);
---@type fontalign
ALIGN_CENTER = (ALIGN_VCENTER | ALIGN_HCENTER);
---@type fontalign


--- Prints text with a font
--- @param font ttf font handle
--- @param x integer X coordinate
--- @param y integer Y coordinate
--- @param fontalign fontalign alignment commands for text
--- @param width  integer defines width  of draw area, text that goes beyond this area is not drawn
--- @param height integer defines height of draw area, text that goes beyond this area is not drawn
--- @param text string the text to be written
--- @param color color **[Optional]**: if not specified default is RGBA #80808080 
--- @overload fun(font:ttf, x:integer, y:integer, fontalign:fontalign, width:integer, height:integer, text:string)
--- @see Color.new
function Font.ftPrint(font, x, y, fontalign, width, height, text, color) end

--- Unloads a font from RAM
--- @param font ttf font handle
function Font.ftUnload(font) end

--- DeInitializes the TTF Fonts system
function Font.ftEnd() end

---Image font loaded by [Font.load](lua://Font.load)
---@alias gsfont integer
---@see Font.load
---@see Font.unload
---@see Font.print

--- Loads a GSFONT from FNT, PNG or BMP files
--- @param path string path to font
--- @return gsfont fonthandle 
--- @nodiscard
function Font.load(path) end

--- Prints text with a GSFONT
--- @param font gsfont GSFONT handle
--- @param x number X coordinate
--- @param y number Y coordinate
--- @param scale number scale
--- @param text string text to print
--- @param color color **[Optional]**: if not specified default is RGBA #80808080 
--- @see Font.load
--- @see Color.new
--- @overload fun(font:gsfont, x:number, y:number, scale:number, text:string)
function Font.print(font, x, y, scale, text, color) end

--- unloads a GSFONT
--- @param font gsfont GSFONT handle
--- @see Font.load
function Font.unload(font) end

--- Loads the OSD system font `rom0:FONTM`  
--- **WARNING:** Some systems, like the PSX-DESR and all Arcade PS2s do not have this file as part of their bios, wich makes this function useless on such machines
function Font.fmLoad() end

--- Prints text with the OSD Font. you must call `Font.fmLoad` before using
--- @param x number X coordinate
--- @param y number Y coordinate
--- @param scale number scaling factor for text
--- @param text string text to print
--- @param color color **[Optional]**: if not specified default is RGBA #80808080 
--- @overload fun(x:number, y:number, scale:number, text:string)
--- @see Color.new
--- @see Font.fmLoad
function Font.fmPrint(x, y, scale, text, color) end

--- unloads the OSD Font from RAM
--- @see Font.fmLoad
function Font.fmUnload() end
--#endregion
