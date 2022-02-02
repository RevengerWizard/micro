-- entity
entity = object:extend({
    x = 0, y = 0,
    w = 3, h = 4,
    rot = 0,
    sx = 1, sy = 1,
    ox = 0, oy = 0,
    px = 0, py = 0,
    type = 'entity',
    name = 'none',
    active = true, visible = true
})

entity.entities = {}

function entity:init()
    if world ~= nil then
        world:add(self, self.x, self.y, self.w, self.h)
    end
end

function entity:add(entity)
    if entity then
        table.insert(self.entities, entity)
    else
        table.insert(self.entities, self)
    end
end

function entity:remove(entity)
    if entity then
        for i, v in ipairs(self.entities) do
            if v == entity then
               table.remove(self.entities, i)
            end
        end
    else
        for i, v in ipairs(self.entities) do
            if v == self then
               table.remove(self.entities, i)
            end
        end
    end
end

function entity:removeAll()
    for i, v in ipairs(self.entities) do
        if world:hasItem(v) then
            world:remove(v)
        end
    end
    self.entities = {}
end

function entity:loadAll()
    for i, v in ipairs(self.entities) do
        if v.load then
            v:load()
        end
    end
end

function entity:updateAll(dt)
    for i, v in ipairs(self.entities) do
        if v.update then
            if v.active then
                v:update(dt)
            end
        end
    end
end

function entity:drawAll()
    for i, v in ipairs(self.entities) do
        if v.draw then
            if v.visible then
                v:draw()
            end
        end
    end
end

function entity.filter(item, other)
    return switch(other.type) {
		solid = function()
			return 'slide'
		end,
        platform = function()
            return 'slide'
        end,
        stair = function()
            return
        end
	}
end