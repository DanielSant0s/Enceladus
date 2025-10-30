--- @meta
--- @file intellisense metadata corresponding to the `Graphics` library inside `src/lua/graphics.cpp`
--- @diagnostic disable

---@class Graphics
Graphics = {}

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
--- @param width integer rectangle height
--- @param color color the color of the rectangle
function Graphics.drawRect(X, Y, width, height, color) end

--- @param X integer X coordinate for begining of line
--- @param Y integer Y coordinate for begining of line
--- @param X2 integer X coordinate for end of the line
--- @param Y2 integer Y coordinate for end of the line
--- @param color color the color of the line
function Graphics.drawLine(X, Y, X2, Y2) end

--- @param X integer X coordinate for circle center
--- @param Y integer Y coordinate for circle center
--- @param radius integer circle radius
--- @param color color circle color
--- @param fill integer **1:**draw a filled circle, **0:**draw only the perimeter of the circle. _Optional parameter: 0 is the default_
--- @overload fun(X:integer, Y:integer, radius:integer, color:color)
function Graphics.drawCircle(X, Y, radius, color, fill) end

function Graphics.drawTriangle() end

function Graphics.drawQuad() end

--- @alias Texture integer

--- Loads a PNG/JPG/BMP image from a file
--- @param path string location of the image to load
--- @param delayed boolean Whether the texture manager will be used or not. **Optional Param:** true by default
--- @return Texture|nil texture
--- @overload fun(path:string):Texture|nil:texture
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
function Graphics.getLoadState() end

--- @return Texture|nil texture texture loaded by threadLoadImage. returns nil if there is no instance of threadLoadImage running
--- @see Graphics.threadLoadImage
--- @see Graphics.getLoadState
function Graphics.getLoadData() end

--- Draws an image on screen with it's original size
--- @param image Texture The image texture
--- @param X integer the X coordinates for the top left corner of the image
--- @param Y integer the Y coordinates for the top left corner of the image
--- @param color color A color created by Color.new. [Optional Parameter]
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

---@enum Filter
---@type Filter
NEAREST = 0;
---@type Filter
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
