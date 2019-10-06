
# https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all#i2c-on-pi



import smbus
from time import sleep
		
		
def main():
	
	# Initialize I2C (SMBus)
	bus = smbus.SMBus(1)
	arduino_addr = 0x7f
	arduino_status = 1  # Make us wait, by default.
	
	while True:
	
		my_move = ord(input("\nGive direction: "))
			# ord(str) gets the ascii value of str, so we can send the
			#	input as an int over I2C.
	
		scoot_camera(bus, my_move, arduino_addr)
		print("Taking picture")
		sleep(2)  # Take a picture
		

def scoot_camera(bus, direction, arduino_addr):
		
		print("Requesting move")
		try:
			bus.write_byte(arduino_addr, direction)
		except OSError:
			print("OSError: Failed to write to specified peripheral")
		sleep(1)
		
		arduino_status = 1
		#print("Arduino_status:", arduino_status)
		entered = False  # Keeps track of whether or not we've entered the wait loop.
		while True:
			try:
				arduino_status = bus.read_byte(arduino_addr)
			except OSError:
				print("OSError: Failed to read from specified peripheral")
				
			if arduino_status == 0:
				break
			
			print("Waiting for Arduino to finish moving", arduino_status)
			sleep(.5)  # Poll every half second.
				
			# We have to go into here at least once. 
			entered = True
				
		print("Arduino_status:", arduino_status)
		if entered:
			entered = False
		else:
			print("Dun dern still #@(%ing up >:\\\n"
				  "Or it simply finished moving too fast because you went from 0 to 256 microsteps without changing the amount of steps it needs to take.")
			exit(1)
	

if __name__ == '__main__':
	main()
		