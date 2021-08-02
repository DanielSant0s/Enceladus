-- LuaPlayer boot script
function dumpDirectory(filelist, directory)
	flist = System.listDirectory(directory)
	for idx, file in ipairs(flist) do
		if file.name ~= "." and file.name ~= ".." and file.name ~= "filelist.txt" then
			fullFile = directory .. "/" .. file.name
			if file.directory then
				dumpDirectory(filelist, fullFile)
			else
				fullFileHandle = io.open(fullFile, "r")
				if fullFileHandle then
					md5sum = System.md5sum(fullFileHandle:read("*a"))
					fullFileHandle:close()
					filelist:write(fullFile .. ", size: " .. file.size .. ", md5: " .. md5sum .. "\r\n")
				end
			end
		end
	end
end

flist = System.listDirectory()
dofiles = { 0, 0, 0 }

for idx, file in ipairs(flist) do
	if file.name ~= "." and file.name ~= ".." then
		if string.lower(file.name) == "script.lua" then -- luaplayer/script.lua
			dofiles[1] = file.name
		end
		if file.directory then
			fflist = System.listDirectory(file.name)
			for fidx, ffile in ipairs(fflist) do
				if string.lower(ffile.name) == "script.lua" then -- app bundle
				        dofiles[2] = file.name.."/"..ffile.name
					System.currentDirectory(file.name)
				end
				if string.lower(ffile.name) == "index.lua" then -- app bundle
					dofiles[2] = file.name.."/"..ffile.name
					System.currentDirectory(file.name)
				end
				
				if string.lower(ffile.name) == "system.lua" then -- luaplayer/System
					dofiles[3] = file.name.."/"..ffile.name
				end
			end
		end
	end
end
done = false
for idx, runfile in ipairs(dofiles) do
	if runfile ~= 0 then
		dofile(runfile)
		done = true
		break
	end
end

if not done then
	print("Boot error: No boot script found, creating filelist.txt...")
	filelist = io.open("filelist.txt", "w")
	dumpDirectory(filelist, System.currentDirectory())
	print("Send the filelist.txt to the Lua Player maintainer for bugfixing.")
	filelist:close()
end