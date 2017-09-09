local print=print
--_ENV = {}
--return _ENV
function __G__TRACKBACK__(msg)
	print("----------------------------------")
	print("LUA ERROR:".. tostring(msg))
	print(debug.traceback())
	print("-----------------------------------")
	return msg
end

function reload(m)
	package.loaded[m] = nil
	require(m)
end

function call(m, f, ...)
	local mod = require(m);
	assert(mod, "can't find module"..m);
	local func = mod[f];
	assert(func, "can't find function"..f.." in "..m)
	
	return func(...)
end