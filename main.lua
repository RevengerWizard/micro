love.graphics.setDefaultFilter('nearest', 'nearest')
love.graphics.setLineStyle('rough')

require('globals')

local bump = require('lib.bump')
local baton = require('lib.baton')

--[[global]]
assets = require('lib.cargo').init('assets')
data = require('lib.cargo').init('data')

input = baton{
	controls = {
		left = { 'key:left', 'key:a' },
		right = { 'key:right', 'key:d' },
		up = { 'key:up', 'key:w', 'key:space' },
		down = { 'key:down', 'key:s' },
		attack = { 'key:x' },
		--reload = {'key:r'},
		f = {'key:t'}
	}
}

world = bump.newWorld(16)
gravity = 9.8

debug = false

function love.load()
	push:setup(200, 200)
	scene.load('main')
	--print(scene.stack[1])
end

function love.resize(w, h)
    push:resize(w, h)
end

local TICK_RATE = 1 / 60
local MAX_FRAME_SKIP = 20
function love.run()
	if love.load then love.load(love.arg.parseGameArguments(arg), arg) end

	-- We don't want the first frame's dt to include time taken by love.load.
	if love.timer then love.timer.step() end

	local lag = 0.0

	-- Main loop time.
	return function()
		-- Process events.
		if love.event then
			love.event.pump()
			for name, a, b, c, d, e, f in love.event.poll() do
				if name == "quit" then
					if not love.quit or not love.quit() then
						return a or 0
					end
				end
				love.handlers[name](a, b, c, d, e, f)
				scene[name](a, b, c, d, e, f) -- handle scene event, if any
			end
		end

		-- Cap number of Frames that can be skipped so lag doesn't accumulate
        if love.timer then lag = math.min(lag + love.timer.step(), TICK_RATE * MAX_FRAME_SKIP) end

        while lag >= TICK_RATE do
            if love.update then love.update(TICK_RATE) end
			scene.update(TICK_RATE)
            lag = lag - TICK_RATE
        end

		if love.graphics and love.graphics.isActive() then
			love.graphics.origin()
			love.graphics.clear(love.graphics.getBackgroundColor())

			if love.draw then love.draw() end
			scene.draw()

			love.graphics.present()
		end

		if love.timer then love.timer.sleep(0.001) end
	end
end