function table.print(tbl, prefix)
	if prefix == nil then
		prefix = ""
	end
	for idx, val in ipairs(tbl) do
		if type(val) == "table" then
			print(prefix..idx..": >>")
			table.print(val, prefix.."   >>")
		elseif type(val) == "boolean" then
			if val then
				print(prefix..idx.." : true")
			else
				print(prefix..idx.." : false")
			end
		else
			print(prefix..idx.." : "..val)
		end
	end
end