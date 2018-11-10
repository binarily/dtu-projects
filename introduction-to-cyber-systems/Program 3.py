#Program 3
import machine

#For calculating Celsius value from byte array
def temp_c(data):
    value = data[0] << 8 | data[1]
    temp = (value & 0xFFF) / 16.0
    if value & 0x1000:
        temp -= 256.0
    return temp

#Create a list of LEDs
ledlist = [machine.Pin(13, machine.Pin.OUT)] #0 for built-in LED
ledlist.append(machine.Pin(15, machine.Pin.OUT)) #0 for built-in LED
ledlist.append(machine.Pin(2, machine.Pin.OUT)) #0 for built-in LED

i2c = machine.I2C(scl=machine.Pin(5), sda=machine.Pin(4)) #Set up a bus
#Test if a bus has anything connected to it
i2c.scan()

#Addresses to get the temperature from - we don't need to set settings
address = 24
temp_reg = 5
#Byte array used to hold temperature
data = bytearray(2)

while True:
    i2c.readfrom_mem_into(address, temp_reg, data) #Read 2 bytes from temp register
    temperature = temp_c(data)
    print(temperature) #Print current temperature - for debug purposes
    #Decide on which lights to turn on depending on temperature
    if temperature < 26.5:
        ledlist[0].on()
        ledlist[1].off()
        ledlist[2].off()
    elif temperature < 28:
        ledlist[1].on()
        ledlist[0].off()
        ledlist[2].off()
    else:
        ledlist[2].on()
        ledlist[0].off()
        ledlist[1].off()