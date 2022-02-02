object = {}
object.__index = object

function object:extend(kob)
  kob = kob or {}
  kob.extends = self
  kob.override = self
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

function love.load()
  --test()
  test3()
  --print('ddd')
end
local e = require('ent')

function test3()
    p = player()
    p:hello()
end

function test2()
  entity = object:extend
  {
      x, y = 0,
      w, h = 0,
      world = 'empty'
  }

  function entity:hello()
    return'hello'
  end

  player = entity:extend
  {
      type = 'nop'
  }

  function player:hello()
    entity:hello()
    print('why')
  end

  p = player()
  p:hello()
end

function test1()
  pairPrinter = object:extend{}

  function pairPrinter:printPairs()
    for i = 0, 20, 1 do
      print(i)
    end
  end

  x = 0
  y = 22
  print(x, y)
  
  point = object:extend{
    x = 0,
    y = 0
  }
  point:implement(pairPrinter)
  
  local p = point{}
  p:printPairs()
end

function test()

    entity = object:extend
    {
        x, y = 0,
        w, h = 0,
        world = 'empty'
    }

    entity.entities = {}

    function entity:init()
      table.insert(self.entities, self)
      if self.world == nil then
          self.world = 'full'
      end
    end

    function entity:load()
      print('entity loaded')
    end

    function entity:load_all()
      for i, v in ipairs(self.entities) do
        v:load()
      end
    end

    function entity:hello()
      print('entity')
    end

    player = entity:extend
    {
        type = 'nop'
    }

    superman = player:extend
    {
      type = 'super'
    }

    function superman:load()
      print('superman loaded')
    end

    function player:hello()
      print('player')
    end

    function player:load()
      print('player loaded'..' '..self.x)
    end

    for i = 1, 20, 1 do
      player({x = i * 2})
    end

    for i = 1, 3, 1 do
      entity({x = i * 2})
    end

    superman({x = 23})

    print(#entity.entities)

    entity:load_all()

    --player({pos = 999}):hello()

    --print(#entity.entities)
    --print(entity.entities[1].s)
end

function love.update(dt)

end

function love.draw()

end