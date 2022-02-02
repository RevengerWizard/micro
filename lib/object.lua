object = {}
object.__index = object

function object:extend(kob)
    kob = kob or {}
    kob.extends = self
    return setmetatable(kob, {
        __index = self,
        __call = function(self, ob)
        ob = setmetatable(ob or {}, {__index = kob})
        local ko, init_fn = kob
        while ko do
            if ko.init and ko.init ~= init_fn then
            init_fn = ko.init
            init_fn(ob)
            end
            ko = ko.extends
        end
        return ob
        end
    })
end

function object:implement(...)
    for _, cls in pairs({...}) do
        for k, v in pairs(cls) do
            if self[k] == nil and type(v) == 'function' then
            self[k] = v
            end
        end
    end
end

return object