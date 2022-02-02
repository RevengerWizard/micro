-- graphics

function color(c)

end

function print()

end

function point(x, y)

end

function line()

end

function rectangle()

end

function circle()

end

function clear()

end

function image()

end

function draw()

end

-- math

function cos()

end

function sin()

end

function rand(min, max)
    min = min or 1
    if max then
        return math.random(min, max)
    else
        return math.random() * min
    end
end

function srand(s)
    math.randomseed(s or 0)
end

pi = math.pi

min = math.min
max = math.max

sqrt = math.sqrt

ceil = math.ceil
floor = math.floor

function mid(x, y, z)
    x, y, z = x or 0, y or 0, z or 0
    if x > y then
        x, y = y, x
    end
  
    return max(x, min(y, z))
end

function clamp(low, n, high)
    return math.min(math.max(low, n), high) 
end

function round(n)
    return n >= 0 and math.floor(n + 0.5) or math.ceil(n - 0.5)
end

-- sound

-- table

function add(a, v)
    if a == nil then
        return
    end
    table.insert(a, v)
end

function del(a, dv)
    if a == nil then
        return
    end
    for i, v in ipairs(a) do
        if v == dv then
            table.remove(a, i)
        end
    end
end

function foreach(a, f)
    if not a then
        return
    end
    for i, v in ipairs(a) do
        f(v)
    end
end
  
function all(a)
    local i = 0
    local n = table.getn(a)
  
    return function()
        i = i + 1
        if i <= n then return a[i] end
    end
end