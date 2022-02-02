local sti = require('lib.sti')

-- map
map = object:extend({
    index = 1,
    level,
})

map.levels = {}

function map:load()
    self.level = sti(self.levels[self.index], {"bump"})

    self.level:bump_init(world)

    self.entities = self.level.layers.entities
    self.collisions = self.level.layers.collisions
    self.stairs = self.level.layers.stairs
    self.platforms = self.level.layers.platforms
    self.tilemap = self.level.layers.tilemap

    self.entities.visible = false
    self.collisions.visible = false
    self.stairs.visible = false
    self.platforms.visible = false

    self:spawn()
end

function map:add(level)
    if type(level) == "table" then
        for i, v in ipairs(level) do
            if type(level[i]) ~= 'string' then
                error('invalid level name')
            end
            table.insert(self.levels, 'maps/'..level[i]..'.lua')
        end          
    else
        table.insert(self.levels, 'maps/'..level..'.lua')
    end
end

function map:previous()
    if #self.levels == 1 then
        return
    end
    self.index = self.index - 1
    self:clear()
    self:load()
end

function map:next()
    if #self.levels == 1 then
        return
    end
    self.index = self.index + 1
    self:clear()
    self:load()
end

function map:clear()
    self.level:bump_removeLayer('collisions')
    self.level:bump_removeLayer('stairs')
    self.level:bump_removeLayer('platforms')
    entity:removeAll()
end

function map:spawn()
    for i, v in ipairs(self.entities.objects) do
        switch(v.type) {
            -- items
            chest = function()
                chest({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            potion = function()
                potion({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            coin = function()
                coin({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            key = function()
                key({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            -- enemies
            barbarian = function()
                barbarian({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            dwarf = function()
                dwarf({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            oldman = function()
                oldman({x = math.round(v.x + v.width / 2), y = math.round(v.y + v.height / 2), name = v.name}):add()
            end,
            default = function()
                return
            end
        }
    end   
end

--function map:update(dt)
    --self.level:update(dt)
--end

function map:draw(x, y)
    self.level:draw()
    --self.map:draw(math.round(x + 128) or 0, math.round(y + 72) or 0)

    --self.level:bump_draw()
end

return map