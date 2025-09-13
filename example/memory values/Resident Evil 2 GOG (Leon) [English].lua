local memory = require("memory")
local baseAddress = memory.GetBaseAddress()

function ReadPlayerState()
  local player = {
    health = 0,
    state = 0,
  }
 
  player.health = memory.ReadAs(baseAddress + 0x594646, "u8") 

  return player
end

function DisplayPlayerState(player)
  local state = "Player health:" .. tostring(player.health) .. "/200"
  console.log(state)
end

while true do
  sleep(1 * 1000)
  
  local player = ReadPlayerState()
  DisplayPlayerState(player)
  
  -- Auto Heal 🤡
  if player.health > 0 and player.health ~= 200 then
      memory.Write(baseAddress + 0x594646, "C8")
  end
end