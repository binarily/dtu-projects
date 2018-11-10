#We assume neopixels are connected as one series

#After modifications:
import machine, neopixel
from time import sleep_ms

#For calculating Celsius value from byte array
def temp_c(data):
    value = data[0] << 8 | data[1]
    temp = (value & 0xFFF) / 16.0
    if value & 0x1000:
        temp -= 256.0
    return temp

#Set up a list of neopixels connected to one pin
ledlist = neopixel.NeoPixel(machine.Pin(12), 3)

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
    sleep_ms(200) #Wait so we don't switch too quickly
    print(temperature) #Print current temperature (for debug purposes)
    #Decide on neopixel settings depending on temperature
    if temperature < 26:
        ledlist[0] = (255,0,0)
        ledlist[1] = (0,0,0)
        ledlist[2] = (0,0,0)
        ledlist.write() #Write given color values to neopixels
        print("Branch 1") #For debug purposes
    elif temperature < 27:
        ledlist[0] = (0,0,0)
        ledlist[1] = (255,255,0)
        ledlist[2] = (0,0,0)
        ledlist.write()
        print("Branch 2")
    else:
        ledlist[0] = (0,0,0)
        ledlist[1] = (0,0,0)
        ledlist[2] = (0,255,0)
        ledlist.write()
        print("Branch 3")

