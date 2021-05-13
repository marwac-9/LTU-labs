function Main()
    local t = {}
    A(t,0)
end

function A(t,n)
    if n == 51 then return end
    if t[n] or n > 15 then
        B(t[n])
    end
    A(t,n+1)
end

function B(j)
    j.x = 50
    j.y = 50
end

Main()