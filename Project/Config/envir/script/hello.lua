local print=print
local a = 0;
for v = 1, 10, 1 do
	a = a + v;
	print(a);
end

function test(x, y)
	local tmp = x + y;
	print(tmp);
end