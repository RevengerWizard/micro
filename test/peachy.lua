-- a modified version of the peachy library, by Josh Perry

local json = require("lib.json")
local cron = require("lib.cron")

local peachy = {}
peachy.__index = peachy

local function _pingpongBounce(self)
    if self.direction == "forward" then
        self.direction = "reverse"
        self.frameIndex = self.frameIndex - 2
    else
        self.direction = "forward"
        self.frameIndex = self.frameIndex + 2
    end
end

local function _checkImageSize(self)
    local imageWidth, imageHeight = self.data.meta.size.w, self.data.meta.size.h
    
    assert(imageWidth == self.image:getWidth(), "Image width metadata doesn't match actual width of file")
    assert(imageHeight == self.image:getHeight(), "Image height metadata doesn't match actual height of file")
end

local function _initializeFrames(self)
    assert(self.data ~= nil, "No JSON data!")
    assert(self.data.meta ~= nil, "No metadata in JSON!")
    assert(self.data.frames ~= nil, "No frame data in JSON!")

    -- Initialize all the quads
    self.frames = {}
    for _, frameData in ipairs(self.data.frames) do
        local frame = {}

        local fd = frameData.frame
        frame.quad = love.graphics.newQuad(fd.x, fd.y, fd.w, fd.h, self.data.meta.size.w, self.data.meta.size.h)
        frame.duration = frameData.frame.duration

        table.insert(self.frames, frame)
    end
end

local function _initializeTags(self)
    assert(self.data ~= nil, "No JSON data!")
    assert(self.data.meta ~= nil, "No metadata in JSON!")
    assert(self.data.meta.frameTags ~= nil, "No frame tags in JSON! Make sure you exported them in Aseprite!")

    self.frameTags = {}

    for _, frameTag in ipairs(self.data.meta.frameTags) do
        local ft = {}
        ft.direction = frameTag.direction
        ft.frames = {}

        for frame = frameTag.from + 1, frameTag.to + 1 do
            table.insert(ft.frames, self.frames[frame])
        end

        self.frameTags[frameTag.name] = ft
    end
end

local function call_onLoop(self)
    if self.callback_onLoop then self.callback_onLoop(unpack(self.args_onLoop)) end
end

function peachy.load(data, image, initTag)
    assert(data ~= nil, "No JSON data!")

    local self = setmetatable({}, peachy)

    if type(data) == 'table' then
        self.data = data
    else
        self.data = json.decode(love.filesystem.read(data))
    end

    self.image = image or love.graphics.newImage(self.data.meta.image)

    _checkImageSize(self)

    _initializeFrames(self)
    _initializeTags(self)

    self.paused = true

    self.tag = nil
    self.tagName = nil
    self.direction = nil

    if initTag then
        self:setTag(initTag)
        self.paused = false
    end

    self.isEvent = false

    return self
end

function peachy:setTag(tag)
    assert(tag, "No animation tag specified!")
    assert(self.frameTags[tag], "Tag "..tag.." not found in frametags!")

    if self.tag == self.frameTags[tag] then
        return
    end

    self.tagName = tag
    self.tag = self.frameTags[self.tagName]
    self.frameIndex = nil
    self.direction = self.tag.direction

    if self.direction == "pingpong" then
        self.direction = "forward"
    end

    self:nextFrame()
end

function peachy:setFrame(frame)
    if frame < 1 or frame > #self.tag.frames then
        error("Frame "..frame.." is out of range of tag '"..self.tagName.."' (1.."..#self.tag.frames..")")
    end

    self.frameIndex = frame

    --self.frame = self.tag.frames[self.frameIndex].frame
    self.frame = self.tag.frames[self.frameIndex]
    self.frameTimer = cron.after(self.frame.duration, self.nextFrame, self)
end

function peachy:getFrame()
    return self.frameIndex
end

function peachy:draw(x, y, rot, sx, sy, ox, oy)
    if not self.frame then
        return
    end

    love.graphics.draw(self.image, self.frame.quad, x or 0, y or 0, rot or 0, sx or 1, sy or 1, ox or 0, oy or 0)
end

function peachy:update(dt)
    assert(dt, "No dt passed into update!")

    if self.paused then
        return
    end

    -- If we're trying to play an animation and it's nil or hasn't been set up
    -- properly then error
    assert(self.tag, "No animation tag has been set!")
    assert(self.frameTimer, "Frame timer hasn't been initialized!")

    -- Update timer in milliseconds since that's how Aseprite stores durations
    self.frameTimer:update(dt * 1000)
end

function peachy:nextFrame()
    local forward = self.direction == "forward"

    if forward then
        self.frameIndex = (self.frameIndex or 0) + 1
    else
        self.frameIndex = (self.frameIndex or #self.tag.frames + 1) - 1
    end

    if self.frameIndex == self.callback_frame and (self.tagName == self.callback_tag) then
        if self.callback_onEvent then self.callback_onEvent() end
    end

    -- Looping
    if forward and self.frameIndex > #self.tag.frames then
        if self.tag.direction == "pingpong" then
            _pingpongBounce(self)
        else
            self.frameIndex = 1
        end
        call_onLoop(self)
    elseif not forward and self.frameIndex < 1 then
        if self.tag.direction == "pingpong" then
            _pingpongBounce(self)
        else
            self.frameIndex = #self.tag.frames
            call_onLoop(self)
        end
    end

    -- Get next frame
    self.frame = self.tag.frames[self.frameIndex]

    self.frameTimer = cron.after(self.frame.duration, self.nextFrame, self)
end

function peachy:pause()
    self.paused = true
end

function peachy:play()
    self.paused = false
end

function peachy:stop(onLast)
    local index = 1
    self.paused = true
    if onLast then index = #self.tag.frames end
    self:setFrame(index)
end

function peachy:onLoop(fn, ...)
    self.callback_onLoop = fn
    self.args_onLoop = {...}
end

function peachy:onEvent(tag, frame, fn)
    --table.insert(self.events, { tag = tag, frame = frame, fn = fn })
    self.callback_onEvent = fn
    self.callback_frame = frame
    self.callback_tag = tag
end

function peachy:togglePlay()
    if self.paused then
        self:play()
    else
        self:pause()
    end
end

function peachy:getWidth()
    return self.data.frames[self.frameIndex].frame.w
end

function peachy:getHeight()
    return self.data.frames[self.frameIndex].frame.h
end

function peachy:getDimensions()
    return self:getWidth(), self:getHeight()
end

return peachy