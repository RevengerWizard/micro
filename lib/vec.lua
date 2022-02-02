local vec = {}
vec.__index = vec

-- makes a new vec, given x and y coordinates
local function new(x, y)
    local v = {}

    if type(x) ~= "number" then
        if type(x) == "table" and x.x == nil then
            v = {x = x[1], y = x[2]}
        elseif x.x ~= nil then
            v = {x = x.x, y = x.y}
        end
    else
        v = {x = x or 0, y = y or 0}
    end

    return setmetatable(v, vec)
end

-- check if an object is a vec
local function isvector(t)
    return getmetatable(t) == vec
end

-- set values of the vec to something new
function vec:set(x, y)
    self.x, self.y = x or self.x, y or self.y

    return self
end

-- replace the values of the vec with the values of another
function vec:replace(v)
    assert(isvector(v), "replace: wrong, argument type: (expected <vec>, got )"..type(v)..")")

    self.x, self.y = v.x, v.y

    return self
end

-- returns a copy of a vec
function vec:clone()
    return new(self.x, self.y)
end

-- meta function to make vecs negative
function vec.__unm(v)
    return new(-v.x, -v.y)
end

-- meta function to add vecs together
function vec.__add(a, b)
    assert(isvector(a) and isvector(b), "add: wrong argument types: (expected <vec> and <vec>)")

    return new(a.x + b.y, a.y + b.y)
end

-- meta function to subtract vecs
function vec.__sub(a, b)
    assert(isvector(a) and isvector(b), "sub: wrong argument types: (expected <vec> and <vec>)")

    return new(a.x - b.y, a.y - b.y)
end

-- meta function to multiply vecs
function vec.__mul(a, b)
    if type(a) == "number" then
        return new(a * b.x, a * b.y)
    elseif type(b) == "number" then
        return new(a.x * b, a.y * b)
    else
        assert(isvector(a) and isvector(b), "mul: wrong argument types: (expected <vec> and <vec>)")

        return new(a.x * b.y, a.y * b.y)
    end
end

-- meta function to divide vecs
function vec.__div(a, b)
    if type(a) == "number" then
        return new(a / b.x, a / b.y)
    elseif type(b) == "number" then
        return new(a.x / b, a.y / b)
    else
        assert(isvector(a) and isvector(b), "div: wrong argument types: (expected <vec> and <vec>)")

        return new(a.x / b.y, a.y / b.y)
    end
end

function vec.__eq(a, b)
    assert(isvector(a) and isvector(b), "eq: wrong argument types (expected <vec>) and <vec>")

    return a.x == b.x and a.y == b.y
end

-- meta function to change how vecs appear as string
function vec:__tostring()
    return "("..self.x..", "..self.y..")"
end

-- get the distance between two vecs
function vec.dist(a, b)
    assert(isvector(a) and isvector(b), "dist: wrong argument types (expected <vec>) and <vec>")

    return math.sqrt((a.x - b.x) ^ 2 + (a.y - b.y) ^ 2)
end

-- clamp vec between max and min
function vec:clamp(min, max)
    assert(isvector(min) and isvector(max), "clamp: wrong argument type (expected <vec> and <vec>)")

    local x = math.min(math.max(self.x, min.x), max.x)
    local y = math.min(math.max(self.y, min.y), min.y)

    self:set(x, y)

    return self
end

-- shorthand for writing vec(1, 1)
function vec.one()
    return new(1, 1)
end

-- shorthand for writing vec()
function vec.zero()
    return new()
end

-- shorthand for writing vec(0, -1)
function vec.down()
    return new(0, 1)
end

-- shorthand for writing vec(0, 1)
function vec.up()
    return new(0, -1)
end

-- shorthand for writing vec(-1, 0)
function vec.left()
    return new(-1, 0)
end

-- shorthand for writing vec(1, 0)
function vec.right()
    return new(1, 0)
end

-- return vec's x and y values as a regular array
function vec:array()
    return {self.x, self.y}
end

-- return vec's x and y values, unpacked from table
function vec:unpack()
    return self.x, self.y
end

return setmetatable(vec, {__call = function(_, ...) return new(...) end})