local memory = require("memory")

-- Ultrawide
local patch = {  -- ConstrainAspectRatio
    pattern = "33 ?? ?? 83 ?? 02 31 ?? ?? 0F ?? ?? ?? ?? 00 00", 
    offset = 0x00, 
    value = "90 90 90 90 90 90",
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