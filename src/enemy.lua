local peachy = require('lib.peachy')

-- enemy
enemy = entity:extend{
    w = 4, h = 4,
	type = 'enemy',
    image,
	data,
	sprite,
    dx = 0, dy = 0,
    damping = 2, decel = 4,
	speed = 35,	-- 45 / 35
	flip = true, move = true, attack = false
}

enemy.enemies = {}

function enemy:load()
    self.data = data.characters[self.name]
	self.image = assets.characters[self.name]
	self.sprite = peachy.load(self.data, self.image, 'idle')
end

function enemy:update(dt)	
	self.sprite:update(dt)

	if input:pressed('f') then self.move = not self.move end

	if input:down('attack') then
		self.dx = 75 * dt
		self.dy = -75 * dt
	end

	-- check left
	local items, lens = world:querySegment(self.x - 3, self.y, self.x - 3, self.y + 7, function(item)
		if item.type == 'solid' or item.type == 'platform' then
			return true
		else
			return
		end
	end)

	-- check right
	local itemss, lenss = world:querySegment(self.x + self.w + 3, self.y, self.x + self.w + 3, self.y + 7, function(item)
		if item.type == 'solid' or item.type == 'platform' then
			return true
		else
			return
		end
	end)

	-- sight left
	local itemsss, lensss = world:querySegment(self.x, self.y + 2, self.x - 10, self.y + 2, function(item)
		if item.type == 'player' then
			return true
		else
			return
		end
	end)

	-- sight right
	local itemssss, lenssss = world:querySegment(self.x + 4, self.y + 2, self.x + 12, self.y + 2, function(item)
		if item.type == 'player' then
			return true
		else
			return
		end
	end)

	if self.sx == -1 then
		if lensss > 0 then
			self.move = false
			self.attack = true
		end
	elseif self.sx == 1 then
		if lenssss > 0 then
			self.move = false
			self.attack = true
		end
	end

	if self.move then
		if lens == 0 then
			self.flip = false
		end

		if lenss == 0 then
			self.flip = true
		end
	end

	if self.move then
		if self.flip then
			-- left
			self.dx = -self.speed * dt
		else
			-- right
			self.dx = self.speed * dt
		end
	else
		self.dx = 0
	end

    self.dx = self.dx / (1 + self.damping * dt)
	self.dy = self.dy / (1 + self.damping * dt)

    self.dy = self.dy + (gravity * dt)

	if self.dx ~= 0 or self.dy ~= 0 then
		local cols, len
		self.x, self.y, cols, len = world:move(self, self.x + self.dx, self.y + self.dy, self.filter)

		-- collisions
		if len > 0 then
			for i = 1, len do
				if cols[i].normal.y == -1 then
					self.dy = 0
				elseif cols[i].normal.y == 1 then
					self.dy = -self.dy / 4
				end
			end
		end
	end

	if not self.attack then
		if self.dx ~= 0 then		
			self.sprite:setTag('run')
		else
			self.sprite:setTag('idle')
		end
	else
		self.sprite:onLoop(function() self.move = true self.attack = false end)
		self.sprite:setTag('attack1')
	end
end

function enemy:draw()
	if self.dx < 0 then
		self.sx = -1
	elseif self.dx > 0 then
		self.sx = 1
	end

    self.sprite:draw(self.x + self.px, self.y + self.py, self.rot, self.sx, self.sy, self.ox, self.oy)

	--love.graphics.rectangle('line', self.x, self.y, self.w, self.h)
	
	--love.graphics.line(self.x - 3, self.y, self.x - 3, self.y + 7)	-- left
	--love.graphics.line(self.x + self.w + 3, self.y, self.x + self.w + 3, self.y + 7)	-- right
	
	--love.graphics.line(self.x, self.y + 2, self.x - 10, self.y + 2)		-- sight left
	--love.graphics.line(self.x + 4, self.y + 2, self.x + 12, self.y + 2)		-- sight right
end

-- barbarian
barbarian = enemy:extend{

}

function barbarian:load()
	enemy.load(self)

	self.ox = math.round(self.sprite:getWidth() / 2) - 4
	self.oy = math.round(self.sprite:getHeight() / 2) + 5

	self.px = 2
end

-- butcher
butcher = enemy:extend{

}

-- caveman
caveman = enemy:extend{

}

-- demon
demon = enemy:extend{

}

-- dwarf
-- dwarf pickaxe
dwarf = enemy:extend{
	speed = 30
}

function dwarf:load()
	enemy.load(self)

	switch(self.name) {
		dwarf = function()
			self.ox = math.int(self.sprite:getWidth() / 2) - 2
			self.oy = math.int(self.sprite:getHeight() / 2) + 3

			self.px = 2
		end,
		dwarf_pickaxe = function()
			self.ox = math.int(self.sprite:getWidth() / 2) - 2
			self.oy = math.int(self.sprite:getHeight() / 2) + 2

			self.px = 2
		end
	}
end

-- guard
guard = enemy:extend{

}

-- knight
-- knight_shield
knight = enemy:extend{

}

-- lizard
lizard = enemy:extend{

}

-- monk
monk = enemy:extend{

}

-- mooseman
mooseman = enemy:extend{

}

-- oldman
oldman = enemy:extend{
	speed = 15
}

function oldman:load()
	enemy.load(self)

	self.ox = math.int(self.sprite:getWidth() / 2) - 2
	self.oy = math.int(self.sprite:getHeight() / 2) + 3

	self.px = 2
end

-- rhino
rhino = enemy:extend{

}

-- small_demon
small_demon = enemy:extend{

}

-- troll
troll = enemy:extend{

}

-- wizard
wizard = enemy:extend{

}