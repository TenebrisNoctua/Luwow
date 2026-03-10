local LuVK = require("LuVK") :: any
local Window = LuVK.new("Window", {
   Title = "Luwow Created Window",
   Size = vector.create(400, 300),
   Position = vector.create(800, 200)
})

Window.initialize()