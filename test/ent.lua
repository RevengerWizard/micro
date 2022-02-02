entity = object:extend
{
    x, y = 0,
    w, h = 0,
    world = 'empty'
}

function entity:hello()
  print'hello'
end

player = entity:extend
{
    type = 'nop'
}

function player:hello()
  entity:hello()
  print('why')
end
