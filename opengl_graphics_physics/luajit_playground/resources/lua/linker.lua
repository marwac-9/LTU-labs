local files = {}
function usefile(n)
    files[n] = true
end

dofile "includes.lua"

for k in pairs(files) do
    require(k)
end