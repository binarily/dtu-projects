#Program 2

import machine

# Create a list of LEDs (ledlist)
ledlist = [machine.Pin(13, machine.Pin.OUT)] #0 for built-in LED
ledlist.append(machine.Pin(15, machine.Pin.OUT)) #0 for built-in LED
ledlist.append(machine.Pin(5, machine.Pin.OUT)) #0 for built-in LED
button = machine.Pin(12, machine.Pin.IN, machine.Pin.PULL_UP) #Input button

#Initialise: turn on the green light and move onto the next light
counter = 0
ledlist[counter].on()
counter += 1

while True: 
    if not button.value():
        ledlist[counter].off() #Turn off current one
        counter = (counter + 1) % 3 #Find the next light
    	ledlist[counter].on() #Turn on the next light
    	while not button.value(): #Wait until button is no longer pressed - prevents quick changes
    		pass

