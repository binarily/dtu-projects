import socket

host = "192.168.4.1"

port = 80

while True:
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host, port))
	pin = input("Select the pin (type e to exit): ").encode('utf-8')
	if pin == b"e":
		s.close()
		exit(0)
	value = input("Set its value: ").encode('utf-8')

	request = b"GET /pins/"+pin+b"/"+value+b" HTTP/1.1\r\nHost: "+host.encode('utf-8')+b"\r\n\r\n"
	print(request)
	s.settimeout(2)
	s.send(request)

	try:
		result = s.recv(10000)
		while(len(result)>0):
			print(result)
			result = s.recv(10000)
	except:
		pass
	s.close()