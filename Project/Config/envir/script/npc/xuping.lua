local test = require "serverd.test"
local print=print
local type=type
local a = 0;
_ENV = {}

function OnStart(x, y)
	local tmp = x + y;
	print("OnStart"..tmp);
	local c = test.TestAdd(1, 2);
	print("Add complete:"..c);
	return c;
end

function OnClose(id)
	print("OnClose"..id)
end

function testAdd(x)
	local tmp = x + 1;
	print("TestAdd"..tmp);
	--local c = test.TestAdd(1, 2);
	--print("Add complete:"..c);
	return tmp;
end

function testNest(x)
	print(type(x));
	if (x == 0) then
		return x;
	end
	local tmp = test.TestNest(x-1);
	return tmp + x;
end

function Add(a, b)
	local tmp = a + b;
	print("Add12"..a.." + "..b);
	newTest();
	return tmp;
end

function newTest()
	print("this is new test");
end

return _ENV