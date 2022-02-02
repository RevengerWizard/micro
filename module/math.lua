-- Clamps a number to within a certain range.
function math.clamp(low, n, high) 
    return math.min(math.max(low, n), high) 
end

function math.round(number)
    return number >= 0 and math.floor(number + 0.5) or math.ceil(number - 0.5)
end

function math.int(number)
    return number >= 0 and math.floor(number) or math.ceil(number)
end

function math.lerp(a, b, x) 
    return a + (b - a) * x 
end