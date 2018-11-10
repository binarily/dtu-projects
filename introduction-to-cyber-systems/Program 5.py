#Program 5 - potentiometer on channel 3 (D1 and D0 high, D2 low - push to D_in - made in line 13)
import machine, neopixel, time, ubinascii

spi = machine.SPI(1, baudrate=1000000, polarity=0, phase=0) 
#Set up chip select
chip_select = machine.Pin(16, machine.Pin.OUT)

#Get a list of 1 neopixel
led = neopixel.NeoPixel(machine.Pin(15), 1)

data_out = bytearray(3)
data_in = bytearray(3)
#Set up input
data_in[0] = 0x01
data_in[1] = 0xb0
data_in[2] = 0x0 
while True:
	chip_select.off()
	spi.write_readinto(data_in, data_out) #Choose third input
	chip_select.on()
	time.sleep_ms(200) #Wait so results don't come too quick
	data_out[1] = data_out[1] & 0x03
	hex = ubinascii.hexlify(data_out) #Get a hexadecimal value from output
	integer = int(hex, 16) #Get a decimal value from output
	fraction = integer % 1024 #Get results from the first 10 bits (0-1023) and dismiss the rest
	print(fraction) #Print the result (for debug)
	led[0] = (int(255*((fraction)/1023)), int(255*((fraction)/1023)), int(255*((fraction)/1023))) #Set up the light
	led.write() #Write to neopixel

