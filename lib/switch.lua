-- https://pastebin.com/MxhirT7Z

switch = function(check)
    return function(case)
        if type(case[check]) == 'function' then
            return case[check]()
        elseif case['default'] and type(case['default'] == 'function') then
            return case['default']()
        else
            return
        end
    end
end

return switch