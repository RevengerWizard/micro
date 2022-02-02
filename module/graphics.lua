local gfx, reg = love.graphics, debug.getregistry()
local ImageData, ParticleSystem, SpriteBatch, Shader = reg.ImageData, reg.ParticleSystem, reg.SpriteBatch, reg.Shader
local clear, getColor, setColor = gfx.clear, gfx.getColor, gfx.setColor
local getBackgroundColor, setBackgroundColor = gfx.getBackgroundColor, gfx.setBackgroundColor
local getPixel, setPixel, mapPixel = ImageData.getPixel, ImageData.setPixel, ImageData.mapPixel
local getParticleColors, setParticleColors = ParticleSystem.getColors, ParticleSystem.setColors
local getBatchColor, setBatchColor = SpriteBatch.getColor, SpriteBatch.setColor
local sendColor = Shader.sendColor

local function round(v)

	return math.floor(v + .5)
end

-- convert a single channel value from [0.0,1.0] to [0,255]
local function channel2byte(c)

	return round(c * 255)
end

-- convert a single channel value from [0,255] to [0.0,1.0]
local function byte2channel(c)

	return c / 255
end

-- convert RGBA values from [0.0,1.0] to [0,255]
local function rgba2bytes(r, g, b, a)

	return round(r * 255), round(g * 255), round(b * 255), a and round(a * 255)
end

-- convert RGBA values from [0,255] to [0.0,1.0]
local function bytes2rgba(r, g, b, a)

	return r / 255, g / 255, b / 255, a and a / 255
end

-- convert RGBA value table from [0.0,1.0] to [0,255]. places the result in dest, if given
local function table2bytes(color, dest)

	dest = dest or {}
	dest[1], dest[2], dest[3], dest[4] = rgba2bytes(color[1], color[2], color[3], color[4])

	return dest
end

-- convert RGBA value table from [0,255] to [0.0,1.0]. places the result in dest, if given
local function bytes2table(color, dest)

	dest = dest or {}
	dest[1], dest[2], dest[3], dest[4] = bytes2rgba(color[1], color[2], color[3], color[4])

	return dest
end

-- convert RGBA values or table from [0.0,1.0] to [0,255]. returns separate values
local function color2bytes(r, g, b, a)

	if type(r) == 'table' then
		r, g, b, a = r[1], r[2], r[3], r[4]
	end

	return rgba2bytes(r, g, b, a)
end

-- convert RGBA values or table from [0,255] to [0.0,1.0]. returns separate values
local function bytes2color(r, g, b, a)

	if type(r) == 'table' then
		r, g, b, a = r[1], r[2], r[3], r[4]
	end

	return bytes2rgba(r, g, b, a)
end

local tempTables = { {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} }

love.graphics.newPixelImage = function(filename)

	local image = love.graphics.newImage(filename)
	image:setFilter('nearest', 'nearest', 1)

	return image
end

love.graphics.getColorBytes = function()

	return rgba2bytes(getColor())
end

love.graphics.setColorBytes = function(r, g, b, a)

	return setColor(bytes2color(r, g, b, a))
end

love.graphics.getBackgroundColorBytes = function()

	return rgba2bytes(getBackgroundColor())
end

love.graphics.setBackgroundColorBytes = function(r, g, b, a)

	return setBackgroundColor(bytes2color(r, g, b, a))
end

love.graphics.clearBytes = function(...)

	local nargs = select('#', ...)
	if nargs == 0 then return clear() end

	local argtype = type(select(1, ...))
	if argtype == 'boolean' then return clear(...) end

	local converter = argtype == 'table' and bytes2table or byte2channel
	local args, i = {...}, 1

	repeat
		args[i] = converter(args[i], tempTables[i])
		i = i + 1
	until type(args[i]) ~= argtype

	return clear(unpack(args))
end

function ImageData:getPixelBytes(x, y)

	return rgba2bytes(getPixel(self, x, y))
end

function ImageData:setPixelBytes(x, y, r, g, b, a)

	return setPixel(self, x, y, bytes2rgba(r, g, b, a))
end

function ImageData:mapPixelBytes(fn)

	return mapPixel(self, function(x, y, r, g, b, a)
		return bytes2rgba(fn(x, y, rgba2bytes(r, g, b, a)))
	end)
end

function ParticleSystem:setColorsBytes(...)

	local args, nargs = {...}, select('#', ...)

	if type(args[1]) == 'table' then
		for i = 1, nargs do
			args[i] = bytes2table(args[i], tempTables[i])
		end
	else
		for i = 1, nargs do
			args[i] = args[i] / 255
		end
	end

	return setParticleColors(self, unpack(args))
end

function ParticleSystem:getColorsBytes()

	local colors = { getParticleColors(self) }
	local ncolors = #colors

	for i = 1, ncolors do
		local rgba = colors[i]
		rgba[1], rgba[2], rgba[3], rgba[4] = rgba2bytes(rgba[1], rgba[2], rgba[3], rgba[4])
	end

	return unpack(colors)
end

function SpriteBatch:getColorBytes()

	local r, g, b, a = getBatchColor(self)

	if r then
		return rgba2bytes(r, g, b, a)
	end
end

function SpriteBatch:setColorBytes(r, g, b, a)

	if r then
		return setBatchColor(self, bytes2color(r, g, b, a))
	end

	return setBatchColor(self)
end

function Shader:sendColorBytes(name, ...)

	local colors, ncolors = {...}, select('#', ...)

	for i = 1, ncolors do
		colors[i] = bytes2table(colors[i], tempTables[i])
	end

	return sendColor(self, name, unpack(colors))
end