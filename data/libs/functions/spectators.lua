--@module Spectators
---@author @Glatharth
---@class Spectators
---@field only_player boolean
---@field multi_floor boolean
---@field blacklist_pos table
---@field creature_detect table
---@field remove_destination Position
---@field check_position table

Spectators = {}
setmetatable(Spectators, {
    __call = function(self)
        local spectators_data = {
            only_player = true,
            multi_floor = false,
            blacklist_pos = {},
            creature_detect = {}
        }
        return setmetatable(spectators_data, {__index = Spectators})
    end
})

--- Get only_player value
---@return boolean
function Spectators.getOnlyPlayer(self)
    return self.only_player
end

--- Set only_player value
---@param boolean boolean
function Spectators.setOnlyPlayer(self, boolean)
    self.only_player = boolean
end

--- Get multi_floor value
---@return boolean
function Spectators.getMultiFloor(self)
    return self.multi_floor
end

--- Set multi_floor value
---@param boolean boolean
function Spectators.setMultiFloor(self, boolean)
    self.multi_floor = boolean
end

--- Set creature_detect value
---@param creatures table
function Spectators.setCreatureDetect(self, creatures)
    self.creature_detect = creatures
end

--- Get creature_detect value
---@return table
function Spectators.getCreatureDetect(self)
    return self.creature_detect
end

--- Remove creature_detect value
function Spectators.removeCreatureDetect(self)
    self.creature_detect = {}
end

--- Set remove_destination value
---@param destination Position
function Spectators.setRemoveDestination(self, destination)
    self.remove_destination = destination
end

--- Get remove_destination value
---@return Position
function Spectators.getRemoveDestination(self)
    return self.remove_destination
end

--- Set check_position value
---@param position table
function Spectators.setCheckPosition(self, position)
    if position.from and position.to then
        self.check_position = position
    else
        error("Error set position.")
    end
end

--- Get check_position value
---@return table
function Spectators.getCheckPosition(self)
    return self.check_position
end

--- Get blacklist_pos value
---@return table
function Spectators.getBlacklistPos(self)
    return self.blacklist_pos
end

--- Set blacklist_pos value
---@param list table
function Spectators.setBlacklistPos(self, list)
    if type(list) == "table" then
        self.blacklist_pos = list
    else
        error("List is not a table")
    end
end

--- Convert position to range
---@return table
function Spectators.convertPosToRange(self)
    local pos = self:getCheckPosition()
    return {
        x = (pos.to.x - pos.from.x) / 2,
        y = (pos.to.y - pos.from.y) / 2,
        z = pos.from.z
    }
end

--- Convert position
---@return Position
function Spectators.convertPos(self)
    local pos = self:getCheckPosition()
    local range = self:convertPosToRange()
    return Position(
        pos.from.x + range.x,
        pos.from.y + range.y,
        range.z
    )
end

--- Check if creature is in blacklist_pos
---@param creature Creature
---@return boolean
function Spectators.checkCreatureBlacklistPos(self, creature)
    local creature_pos = creature:getPosition()
    for _, v in pairs(self:getBlacklistPos()) do
        if creature_pos.x >= v.from.x and creature_pos.x <= v.to.x then
            if creature_pos.y >= v.from.y and creature_pos.y <= v.to.y then
                return true
            end
        end
    end
    return false
end

--- Remove monsters
function Spectators.removeMonsters(self)
    if self:getCreatureDetect() then
        for _, v in pairs(self:getCreatureDetect()) do
            if v:isMonster() then
                v:remove()
            end
        end
    end
end

--- Remove player
---@param player Player
function Spectators.removePlayer(self, player)
    if player then
        if player:isPlayer() then
            local destination = self:getRemoveDestination()
            if destination then
                player:teleportTo(destination)
            else
                player:teleportTo(player:getTown():getTemplePosition())
                player:remove()
            end
        end
    end
end

--- Remove players
---@param players table
function Spectators.removePlayers(self, players)
    local creature_remove = players or self:getCreatureDetect()
    for _, v in pairs(creature_remove) do
        self:removePlayer(v)
    end
end

--- Check spectators
---@param pos table
---@return table
function Spectators.check(self, pos)
    if pos ~= nil then
        self:setCheckPosition(pos)
    end
    local range = self:convertPosToRange()
    pos = self:convertPos()
    local specs = Game.getSpectators(pos, self:getMultiFloor(), self:getOnlyPlayer(), range.x, range.x, range.y, range.y)
    self:setCreatureDetect(specs)
    return specs
end

--- Get players count
---@return number
function Spectators.getPlayers(self)
    local count = 0
    if not self:getCreatureDetect() then
        error('Not creature detect')
        return nil
    end
    for _, v in pairs(self:getCreatureDetect()) do
        if v:isPlayer() then
            count = count + 1
        end
    end
    return count
end

--- Clear creatures cache
function Spectators.clearCreaturesCache(self)
    self:removeCreatureDetect()
end
