local memory = require("memory")
local baseAddress = memory.GetBaseAddress()

function ReadPlayerState()
  local player = {
    health = 0,
    state = 0,
  }
 
  player.health = memory.ReadAs(baseAddress + 0x65CCC0, "u8") 
  player.state = memory.ReadAs(baseAddress + 0x65CCC7, "u8")

  return player
end

function DisplayPlayerState(player)
  local state = "Player health:" .. tostring(player.health) .. "/200"
  
  if player.state == 6 then
    state = state .. " (Poisoned)"
  elseif player.state == 5 then
    state = state .. " (Virus)"
  end
  
  console.log(state)
end

while true do
  sleep(1 * 1000)
  
  local player = ReadPlayerState()
  DisplayPlayerState(player)
  
  -- Auto Heal 🤡
  if player.health > 0 and player.health ~= 200 then
      memory.Write(baseAddress + 0x65CCC0, "C8")
  end
end