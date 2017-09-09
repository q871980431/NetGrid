local test = require "serverd.test"
local print=print
local a = 0;
_ENV = {}

function OnStart(x, y)
	local tmp = x + y;
	print("OnStart"..tmp);
	local c = test.TestAdd(1, 2);
	print("Add complete:"..c);
end

function OnClose(id)
	print("OnClose"..id)
end


return _ENV