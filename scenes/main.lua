local main = {}

function main.load()
    map:add({'platform'})
    map:load()

    p = player({x = 122, y = 72, name = 'basic_green', input = input})
    p:add()

    entity:loadAll()
end

function main.unload()
    map:clear()
    entity:removeAll()
end

function main.update(dt)
    entity:updateAll(dt)

    if p.y > 250 then
        scene.load('main')
    end
end

function main.draw()
    push:start()
        love.graphics.clear(41 / 255, 30 / 255, 49 / 255)
        map:draw()
        entity:drawAll()
    push:finish()
end

function main.keypressed(key)
    if key == 'f' then
        push:switchFullscreen()
    end

    if key == 'r' then
        scene.load('main')
    end
end

return main