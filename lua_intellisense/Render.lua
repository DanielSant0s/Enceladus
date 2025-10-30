---@meta
---@file intellisense metadata corresponding to the `Timer` library inside `src/lua/timer.cpp`
---@diagnostic disable


---@class Render
Render = {}
---@class Camera
Camera = {}
---@class Lights
Lights = {}

--- Initialize 3d rendering
--- @param aspect integer the aspect ratio. pass either `4/3` or `16/9`
function Render.init(aspect) end

---@type Model model

--- Loads a wavefront OBJ
--- @param model string path to .OBJ file
--- @param texture Texture Texture object from `Graphics.loadImage`
--- @return Model model 
--- @see Graphics.loadImage
--- @overload fun(model:string): model:Model
function Render.loadOBJ(model, texture) end

--- Draws the 3d model
--- @param model Model
--- @param pos_x integer Coordinate for the X axis
--- @param pos_y integer Coordinate for the Y axis
--- @param pos_z integer Coordinate for the Z axis
--- @param rot_x integer Rotation   for the X axis
--- @param rot_y integer Rotation   for the Y axis
--- @param rot_z integer Rotation   for the Z axis
function Render.drawOBJ(model, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z) end

--- Draws the 3d model
--- @param model Model
--- @param pos_x integer Coordinate for the X axis
--- @param pos_y integer Coordinate for the Y axis
--- @param pos_z integer Coordinate for the Z axis
--- @param rot_x integer Rotation   for the X axis
--- @param rot_y integer Rotation   for the Y axis
--- @param rot_z integer Rotation   for the Z axis
--- @param color color color
--- @see Color.new
function Render.drawBbox(model, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, color) end

--- Frees the 3D model, just in case it's not obvious: the texture is not freed
--- @param model Model
function Render.freeOBJ(model) end

---change camera position
---@param X integer Coordinate
---@param Y integer Coordinate
---@param Z integer Coordinate
function Camera.position(X, Y, Z) end

---change camera rotation
---@param X integer Rotation angle
---@param Y integer Rotation angle
---@param Z integer Rotation angle
function Camera.rotation(X, Y, Z) end

AMBIENT = 0;
DIRECTIONAL = 1;

--- @param id integer
---@param dir_x number
---@param dir_y number
---@param dir_z number
---@param type number
---@param r number
---@param g number
---@param b number
function Lights.set(id, dir_x, dir_y, dir_z, type, r, g, b) end

--- @param count integer the ammount of lights
function Lights.create(count) end
