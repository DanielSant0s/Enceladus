-- do a `cat deskboot.lua boot.lua > tmp.lua && lua tmp.lua && rm tmp.lua` to test on desktop
System = {}
function table.print(tbl, prefix)
	if not prefix then
		prefix = ""
	end
	for idx, val in ipairs(tbl) do
		if type(val) == "table" then
			table.print(val, prefix.."  ")
		elseif type(val) == "boolean" then
			print(idx.." : ")
		else
			print(idx.." : "..val)
		end
	end
end

function System.listDirectory(foo)
	d = { ["."] = {
			{name="EBOOT.PBP", directory= false}, 
			{name="mygame", directory = true},
			{name="SCRIPT.LUA", directory = false}
			},
		  ["mygame"] = {
		  	{name="INDEX.LUA", directory = false},
		  	{name="img.png", directory = false}
		  	}
		  }
	return d[foo] 
end

function System.currentDirectory(foo)
end
