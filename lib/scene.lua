local scene = {stack = {}}
scene.__index = scene

setmetatable(scene,
{__index = function(t,k)
    if current and type(current[k]) == "function" then
        return current[k]
    end
    return function() end
end})

function scene.load(name)
    if scene.stack[#scene.stack] ~= name then
        scene.stack[#scene.stack + 1] = name
    end

    if current then scene.unload() end
    
    local chunk = love.filesystem.load("scenes/"..name..".lua")
    if not chunk then 
        error("Attempt to load scene \""..name.."\", but it was not found in \""..'scenes'.."\" folder.") 
    end
    
    current = chunk()
    collectgarbage()
    current.load()
end

function scene.exit()
    if #scene.stack == 1 then
        return
    end
    table.remove(scene.stack,#scene.stack)
	scene.load(scene.stack[#scene.stack])
end

return scene