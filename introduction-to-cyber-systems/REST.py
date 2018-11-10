import machine, time, ubinascii, json
pins = [machine.Pin(i, machine.Pin.OUT) for i in (0, 2, 4, 5, 12, 13, 14, 15)]

dictionary = {0:0, 2:1, 4:2, 5:3, 12:4, 13:5, 14:6, 15:7}

html = """<!DOCTYPE html>
<html>
    <head> <title>ESP8266 Pins</title> </head>
    <body> <h1>ESP8266 Pins</h1>
        <table border="1"> <tr><th>Pin</th><th>Value</th></tr> %s </table>
    </body>
</html>
"""

#Temperature-specific part
#For calculating Celsius value from byte array
def temp_c(data):
    value = data[0] << 8 | data[1]
    temp = (value & 0xFFF) / 16.0
    if value & 0x1000:
        temp -= 256.0
    return temp

i2c = machine.I2C(scl=machine.Pin(5), sda=machine.Pin(4)) #Set up a bus
#Test if a bus has anything connected to it
i2c.scan()

#Addresses to get the temperature from - we don't need to set settings
address = 24
temp_reg = 5
#Byte array used to hold temperature
data = bytearray(2)

#Potentiometer-specific part
spi = machine.SPI(1, baudrate=1000000, polarity=0, phase=0) 
#Set up chip select
chip_select = machine.Pin(16, machine.Pin.OUT)

data_out = bytearray(3)
data_in = bytearray(3)
#Set up input
data_in[0] = 0x01
data_in[1] = 0xb0
data_in[2] = 0x0 

#Function for regular requests
def webpage_request():
    #Get the temperature
    i2c.readfrom_mem_into(address, temp_reg, data) #Read 2 bytes from temp register
    temperature = temp_c(data)
    #Get the potentiometer
    chip_select.off()
    spi.write_readinto(data_in, data_out) #Choose third input
    chip_select.on()
    data_out[1] = data_out[1] & 0x03
    hex = ubinascii.hexlify(data_out) #Get a hexadecimal value from output
    integer = int(hex, 16) #Get a decimal value from output
    fraction = integer % 1024 #Get results from the first 10 bits (0-1023) and dismiss the rest
    rows = ['<tr><td>%s</td><td>%d</td></tr>' % (str(p), p.value()) for p in pins]
    rows.append('<tr><td>%s</td><td>%d</td></tr>' % ("Temperature", temperature))
    rows.append('<tr><td>%s</td><td>%d</td></tr>' % ("Potentiometer", fraction))
    response = html % '\n'.join(rows)
    return response

def pin_request(value):
    if value == -1:
        rows = [(str(p), p.value()) for p in pins]
        response = json.dumps(rows)
    else:
        try:
            response = json.dumps((pins[value], pins[value].value()))
        except Exception:
            response = "HTTP/1.1 404 Not Found\r\n"
    return response

def sensor_request(value):
    #Get the temperature
    i2c.readfrom_mem_into(address, temp_reg, data) #Read 2 bytes from temp register
    temperature = temp_c(data)
    #Get the potentiometer
    chip_select.off()
    spi.write_readinto(data_in, data_out) #Choose third input
    chip_select.on()
    data_out[1] = data_out[1] & 0x03
    hex = ubinascii.hexlify(data_out) #Get a hexadecimal value from output
    integer = int(hex, 16) #Get a decimal value from output
    fraction = integer % 1024 #Get results from the first 10 bits (0-1023) and dismiss the rest
    if value == b"":    
        response = json.dumps([("Temperature", temperature), ("Potentiometer", fraction)])
    elif value == b"temperature":
        response = json.dumps(("Temperature", temperature))
    elif value == b"potentiometer":
        response = json.dumps(("Potentiometer", fraction))
    else:
        response = "HTTP/1.1 404 Not Found\r\n"
    return response

def pin_set(pin, value):
    try:
        if value == b"1":
            pins[pin].on()
            response = "HTTP/1.1 200 OK\r\n"
        elif value == b"0":
            pins[pin].off()
            response = "HTTP/1.1 200 OK\r\n"
        else:
            response = "HTTP/1.1 400 Bad Request\r\n"
    except Exception:
        response = "HTTP/1.1 404 Not Found\r\n"
    return response

import socket
addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]

s = socket.socket()
s.bind(addr)
s.listen(1)

print('listening on', addr)

while True:
    cl, addr = s.accept()
    print('client connected from', addr)
    cl_file = cl.makefile('rwb', 0)
    line = cl_file.readline()
    print(line)
    url = line.split()
    if len(url) < 1:
        response = "HTTP/1.1 404 Not Found\r\n"
    elif url[0] == b"GET":
        print(url[1])
        if url[1] == b"/":
            response = webpage_request()
        else:
            path = url[1].split(b"/")
            if path[1] == b"pins":
                if len(path) == 2:
                    response = pin_request(-1)
                elif len(path) == 3:
                    try:
                        response = pin_request(dictionary[int(path[2])])
                    except Exception:
                        response = "HTTP/1.1 404 Not Found\r\n"
                elif len(path) == 4:
                	print("Setting a pin")
                	value = path[3]
                	print(b"value: "+value)
                	try:
                		response = pin_set(dictionary[int(path[2])], value)
                	except Exception:
                		response = "HTTP/1.1 404 Not Found\r\n"
                else:
                	response = "HTTP/1.1 400 Bad Request\r\n"
            elif path[1] == b"sensors":
                if len(path) == 2:
                    response = sensor_request(b"")
                elif len(path) == 3:
                    response = sensor_request(path[2])
                else:
                	response = "HTTP/1.1 400 Bad Request\r\n"
    else:
        response = "HTTP/1.1 404 Not Found\r\n"
    while True:
        line = cl_file.readline()
        print(line)
        if not line or line == b'\r\n':
            break

    cl.send(response)
    cl.close()