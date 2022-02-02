local title = {}

function title.load()
end

function title.unload()
end

function title.update(dt)

end

function title.draw()
    love.graphics.rectangle('line', 100, 100, 16, 32)
end

function title.keypressed(key)
    if key == 'n' then
        scene.load('main')
    end
end

return title