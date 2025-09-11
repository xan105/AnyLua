local memory = require("memory")

local patch = { -- Skip Intro
    pattern = "0F 94 ?? ?? ?? 89 ?? ?? 48 89 ?? ?? ?? E8 ?? ?? ?? ??", 
    offset = 0x01, 
    value = "95",
}

function apply(patch)
  local address, err = memory.Find(patch.pattern)
  if err then
    error(err.message)
  end
  local success, err = memory.Write(address + patch.offset, patch.value)
  if err then
    error(err.message)
  end
  return success
end

if apply(patch) then
  console.log("Applied patch!)
end