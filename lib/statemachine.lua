local statemachine = {}
statemachine.__index = statemachine

local transition = {
    enter = "_enter",
    none = "",
    exit = "_exit"
}

function statemachine:new(state_table, name)
    assert(type(state_table) == "table", "state_table parameter must be a table!")
    assert(type(name) == "string" or type(name) == "nil", "name parameter must be of type string or nil!")
    self:reset()
    self.state_table = state_table
    self:change(name)
end

function statemachine:reset()
    self.state = nil
    self.state_name = nil
    self.prev_name = nil
    self.transition = transition.none
end

-- Takes a state name and a transition type and returns true if 
-- it is able to set the state to that state name and transition type
-- Otherwise, it returns false
--
function statemachine:set_transition_function(new_state_name, transition_type)
    assert(type(new_state_name) == "string" or type(new_state_name) == "nil", "new_state_name parameter must be of type string or nil!")
    assert(type(transition_type) == "string", "transition_type parameter must be a string!")

    if new_state_name == nil or self.state_table[new_state_name .. transition_type] == nil then
        self.state = nil
        self.transition = transition.none
        return false
    else
        self.state = self.state_table[new_state_name .. transition_type]
        assert(type(self.state) == "function", new_state_name .. " must be a function. Is this really a state?")
        -- print("prev: " .. (self.state_name or "") .. self.transition .. " current: " .. (new_state_name .. transition_type or ""))
        self.transition = transition_type
        return true
    end

end

function statemachine:change(new_state_name, immediate)
    --If were already in the same state, just return for now
    if new_state_name == self.state then return end

    --See if the previous state has an exit function
    if not immediate and self:set_transition_function(self.state_name, transition.exit) then
    elseif self:set_transition_function(new_state_name, transition.enter) then
    else self:set_transition_function(new_state_name, transition.none) 
    end

    --Store the previous state name and set the current one
    self.prev_name = self.state_name
    self.state_name = new_state_name    
end

function statemachine:update(dt)
    if self.transition == transition.exit then
        self.state(self.state_table, dt)
        if self:set_transition_function(self.state_name, transition.enter) then 
        else self:set_transition_function(self.state_name, transition.none) 
        end
    elseif self.transition == transition.enter then
        self.state(self.state_table, dt)
        self:set_transition_function(self.state_name, transition.none) 
    elseif self.state ~= nil then
        self.state(self.state_table, dt)
    end
end

return statemachine