status = jit.status()
print(status)
--jit.off()
print(jit.version)
print(jit.version_num)
--jit.debug(0)
print("is jit not nil " .. tostring(jit))
print(jit)
io.write(jit.util)
print("end")
--print(jit.util.status)
io.write(tostring(jit.debug))


function callback(buf)
    io.write('old callback')
    local success, err = xpcall(testfunction('tralalala'),debug.traceback)
    if not success then
        print("error")
        print(err)
    end
    return #buf
end

function testfunction()
    io.write("test function")
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
    j.y = 60
end

function testfunction2(whatToPrint)
    print(whatToPrint)
    mycalcfunc(3,2)
end

function mycalcfunc(num1, num2)
    print(num1+num2)
end

function callbackNew(buf)
    print('before')
    print(buf:op())
    print('after')
    return 0
end

-- Only allow hwaddr with these prefixes
local allowed_prefix = { '', '\5\3' }
local function acl(hwaddr)
    for i,v in ipairs(allowed_prefix) do
        if hwaddr:sub(0, #v) == v then
            return true
        end
    end
    return false
end
-- Flip some bits in the buffer and return length
local lease = {}
function callbackNewTwo(buf, len)
    if buf:op() == 0x01 then -- DHCPDISCOVER
        buf:op(2) -- DHCPOFFER
        local hwaddr = buf:chaddr()
        if not acl(hwaddr) then return 0 end
        local client_ip = lease[hwaddr]
        if client_ip then -- keep leased addr
            buf:yiaddr(client_ip)
        else -- stub address range (:
            client_ip = '192.168.1.1'
            buf:yiaddr(client_ip)
            lease[hwaddr] = client_ip
        end
    end
    return len
end

--stats = jit.util.stats(callback)
--print(stats)

address = '255.255.255.255'
port = 67

-- test.lua
-- Receives a table, returns the sum of its components.
io.write("The table the script received haz:\n");
x = 0
for i = 1, #foo do
  print(i, foo[i])
  x = x + foo[i]
end
io.write("Returning data back to C\n");
return x
