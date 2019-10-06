
# https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all#i2c-on-pi

import smbus
from time import sleep
		
		
def main():
	
	# Initialize I2C (SMBus)
	bus = smbus.SMBus(1)
	arduino_addr = 0x7f
	arduino_status = 1  # Make us wait, by default.
	entered = False
	
	
	while True:
	
		my_move = ord(input("\nGive direction: "))
			# ord(str) gets the ascii value of str, so we can send the
			#	input as an int over I2C.
	
		print("Requesting move")
		try:
			bus.write_byte(arduino_addr, my_move)
		except OSError:
			print("OSError: Failed to write to specified peripheral")
			arduino_status = 1
		sleep(1)
		
		
		try:
			arduino_status = bus.read_byte(arduino_addr)
		except OSError:
			print("OSError: Failed to read from specified peripheral")
			arduino_status = 1
			
		print("Arduino_status:", arduino_status)
		while arduino_status == 1:
			print("Waiting for Arduino to finish moving", arduino_status)
			sleep(.5)  # Poll every half second
			
			try:
				arduino_status = bus.read_byte(arduino_addr)
			except OSError:
				print("OSError: Failed to read from specified peripheral")
				
			# We have to go into here at least once (actually four times, but whatever)
			entered = True
				
		print("Arduino_status:", arduino_status)
		if entered:
			entered = False
		else:
			print("Dun dern still #@(%ing up >:\\")
			break
		
		print("Taking picture")
		sleep(2)  # Take a picture


if __name__ == '__main__':
	main()
		