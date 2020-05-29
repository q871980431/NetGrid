local print=print
local io=io
local hello = require("hello")
local mydum = require("mydump")

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
	print("****************reload:"..m.."**************")
	local oldMod = package.loaded[m] or {}
	package.loaded[m] = nil
	require(m)
	
	local newMod = package.loaded[m]
	for k, v in pairs(newMod) do
		oldMod[k] = v
	end
	package.loaded[m] = oldMod;
	print("*******************load hello*************************");
	local hello = require("mydump")
	hello.print_dump(hello)
	--print("hello.a="..hello.a);
end

function call(m, f, ...)
	local mod = require(m);
	assert(mod, "can't find module"..m);
	local func = mod[f];
	assert(func, "can't find function"..f.." in "..m)
	
	return func(...)
end

function ptfmod(m)
	print("mod name:", m)
	local mod = require(m)
	print_dump(mod)
end

function print_dump(data, showMetatable, lastCount)
    if type(data) ~= "table" then
        --Value
        if type(data) == "string" then
            io.write("\"", data, "\"")
        else
            io.write(tostring(data))
        end
    else
        --Format
        local count = lastCount or 0
        count = count + 1
        io.write("{\n")
        --Metatable
        if showMetatable then
            for i = 1,count do 
                io.write("\t") 
            end
            local mt = getmetatable(data)
            io.write("\"__metatable\" = ")
            print_dump(mt, showMetatable, count)    -- 如果不想看到元表的元表，可将showMetatable处填nil
            io.write(",\n")        --如果不想在元表后加逗号，可以删除这里的逗号
        end
        --Key
        for key,value in pairs(data) do
            for i = 1,count do 
                io.write("\t") 
            end
            if type(key) == "string" then
                io.write("\"", key, "\" = ")
            elseif type(key) == "number" then
                io.write("[", key, "] = ")
            else
                io.write(tostring(key))
            end
            print_dump(value, showMetatable, count)    -- 如果不想看到子table的元表，可将showMetatable处填nil
            io.write(",\n")        --如果不想在table的每一个item后加逗号，可以删除这里的逗号
        end
        --Format
        for i = 1,lastCount or 0 do 
            io.write("\t") 
        end
            io.write("}")
    end
    --Format
    if not lastCount then
        io.write("\n")
    end
end
