local peachy = require('lib.peachy')
local json = require('lib.json')

-- player
player = entity:extend{
	dx = 0, dy = 0,
	initVel = 3, -- 4 / 3
	speed = 45,
	damping = 2, maxVel = 3.5, -- 2; 3.5
	decel = 4,
	input,
	weapon,
	image,
	data,
	sprite,
	type = 'player'
}

player.players = {}

function player:init()
	self.isJump = false
	self.isGround = false
	self.isClimb = false
end

function player:load()
	--self.weapon = sword({})

	if self.weapon then
		self.data = data.characters[self.name..'_'..self.weapon.name]
		self.image = assets.characters[self.name..'_'..self.weapon.name]
	
		self.sprite = peachy.load(self.data, self.image, 'idle')
	else
		self.data = data.characters[self.name]
		self.image = assets.characters[self.name]
	
		self.sprite = peachy.load(self.data, self.image, 'idle')
	end

	self.ox = math.int(self.sprite:getWidth() / 2)
	self.oy = math.int(self.sprite:getHeight() / 2)

	self.px = 1
	self.py = -3
end

function player:update(dt)
	self.sprite:update(dt)
	self.input:update(dt)

	self.isClimb = false

	if input:pressed('attack') then
		self.dx = 75 * dt
		self.dy = -75 * dt
	end

	-- interaction
	local items, len = world:queryRect(self.x, self.y, self.w, self.h, function(item)
		if item.type == 'player' then 
			return 
		else
			return true
		end
	end)

	if len > 0 then
		for i = 1, len do
			--print(items[i].type)
			if items[i].type == 'stair' then
				self.isClimb = true
			end
			
			if items[i].type == 'chest' then
				items[i]:open()
			elseif items[i].type == 'potion' then
				items[i]:pick()
			end
		end
	end

	if not self.isClimb then
		if self.input:down('left') then
			self.dx = -self.speed * dt
		elseif self.input:down('right') then
			self.dx = self.speed * dt
		end
	
		if not self.input:down('left') and (self.dx < 0) then
			self.dx = math.min((self.dx + self.decel * dt), 0)
		elseif not self.input:down('right') and (self.dx > 0) then
			self.dx = math.max((self.dx - self.decel * dt), 0)
		end
	
		-- this block locks in our velocity to maxVel
		local v = math.sqrt(self.dx ^ 2 + self.dy ^ 2)
		if v > self.maxVel then
			local vs = self.maxVel / v
			self.dx = self.dx * vs
			self.dy = self.dy * vs
		end
	
		-- these 2 lines handle damping (aka friction)
		self.dx = self.dx / (1 + self.damping * dt)
		self.dy = self.dy / (1 + self.damping * dt)
	
		if self.input:pressed('up') and (not self.isJump) and (self.isGround) then
			self.isJump = true
			self.isGround = false
			self.dy = -self.initVel
		elseif not self.input:pressed('up') and (self.isJump) then
			self.isJump = false
		end
		
		self.dy = self.dy + (gravity * dt)
	else
		if self.input:down('left') then
			self.dx = -self.speed * dt
		elseif self.input:down('right') then
			self.dx = self.speed * dt
		else
			self.dx = 0
		end
	
		if self.input:down('up') then
			self.dy = -self.speed * dt
		elseif self.input:down('down') then
			self.dy = self.speed * dt
		else
			self.dy = 0
		end
	end
	
	if self.dx ~= 0 or self.dy ~= 0 then
		local cols, len

		self.x, self.y, cols, len = world:move(self, self.x + self.dx, self.y + self.dy, self.filter)
		
		-- collisions
		if len > 0 and (not self.isJump) and (not self.isClimb) then
			for i = 1, len do
				if cols[i].normal.y == -1 then
					self.dy = 0
					self.isGround = true
					self.isJump = false
				elseif cols[i].normal.y == 1 then
					self.dy = -self.dy / 4
					self.isGround = false
					self.isJump = true
				end
				if cols[i].normal.x ~= 0 then
					self.dx = 0
				end
			end
		else
			self.isGround = false
		end
  	end

	if self.dx ~= 0 then
		self.sprite:setTag('run')
	else
		self.sprite:setTag('idle')
	end
end

function player:draw()
	if self.dx < 0 then
		self.sx = -1
	elseif self.dx > 0 then
		self.sx = 1
	end

	self.sprite:draw(math.int(self.x + self.px), math.int(self.y + self.py), self.rot, self.sx, self.sy, self.ox, self.oy)
	--love.graphics.rectangle('line', math.int(self.x), math.int(self.y), self.w, self.h)
end