-- item
item = entity:extend{
    type = 'item',
    sprite,
    dx = 0, dy = 0,
    damping = 2
}

item.items = {}

function item:init()
    self.w = 4
    self.h = 4
end

function item:load()
    self.sprite = assets.items[self.name]
    
    self.ox = math.round((self.sprite:getWidth() / 2) - 1)
    self.oy = math.round((self.sprite:getHeight() / 2) - 1)
end

function item:update(dt)
	self.dy = self.dy / (1 + self.damping * dt)
    
    self.dy = self.dy + (gravity * dt)

    self.x, self.y = world:move(self, self.x, self.y + self.dy, self.filter)
end

function item:draw()
    love.graphics.draw(self.sprite, self.x, self.y, self.rot, self.sx, self.sy, self.ox, self.oy)
    --love.graphics.rectangle('line' ,self.x, self.y, self.w, self.h)
end

-- chest
chest = item:extend{
    type = 'chest',
    empty, full
}

function chest:load()
    item.load(self)

    self.empty = assets.items[self.name..'_empty']
    self.full = assets.items[self.name..'_full']
end

function chest:open()
    self.sprite = self.full
    world:remove(self)
    self.active = false
end

-- coin
coin = item:extend{
    type = 'coin',
    image,
}

function coin:load()
    self.image = assets.items[self.name]
    self.sprite = self.image
end

-- key
key = item:extend{
    type = 'key',
}

function key:update(dt)
end

-- potion
potion = item:extend{
    type = 'potion',
}

function potion:pick()
    entity:remove(self)
    world:remove(self)
end

-- weapon
weapon = item:extend{
    type = 'weapon'
}