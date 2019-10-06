
# https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all#i2c-on-pi


from sys import exit
import smbus
from time import sleep
		
		
def main():
	'''
	print(type(ord('a')))
	testing_str = input("Enter a number: ")
	print("1", type(testing_str), testing_str)
	testing = int(testing_str)
	print("2", type(testing), testing)
	print("3 {0:b}".format(testing))
	print(testing)
	'''

	# Initialize I2C (SMBus)
	bus = smbus.SMBus(1)
	arduino_addr = 0x7f
	arduino_status = 1  # Make us wait, by default.
	
	
	'''
		Disable motors
	'''
	# Disable the motors so we can manually manipulate the machine.
	print("Disabling motors")
	try:
		bus.write_byte(arduino_addr, ord('q'))
		sleep(.1)
		bus.write_byte(arduino_addr, ord('q'))
		sleep(.1)
		bus.write_byte(arduino_addr, ord('q'))
		sleep(.1)
		bus.write_byte(arduino_addr, ord('q'))
		sleep(.1)
		bus.write_byte(arduino_addr, ord('q'))
		sleep(.1)
			# For some reason, sending first 'q' is unreliable. Delay does not fix,
			#	so sending multiple q's with small delays.
			# Sending 'q' or other letters doesn't appear to be the slightest problem
			#	later on, thankfully.
	except OSError:
		print("OSError: Failed to disable motor drivers")
		exit(1)
	
	print("Make sure the Arm is rotated such that the motor wires won't twist and Slider is at bottom-most position")
	
	
	'''
		Get number of pictures per rotation and number of levels of pictures
		from user.
	'''
	# Input from user: how many pictures to take each rotation.
	while True:
		try:
			num_photos_per_rev = int(input("Number of pictures to take per rotation: "))
			break  # exit the loop if the input successfully converted to a number
		except ValueError:
			print("Invalid input: num_photos_per_rev. Numbers only please.")
	
	# How many levels of pictures to take, how many rotations
	while True:
		try:
			num_levels = int(input("Number of levels/rotations to make: "))
			break  # exit the loop if the input successfully converted to a number
		except ValueError:
			print("Invalid input: num_levels. Numbers only please.")
	
	# Send both to Arduino.
	print("Sending input to arduino")
	try:
		bus.write_i2c_block_data(arduino_addr, 0, [num_photos_per_rev, num_levels])
	except OSError:
		print("OSError: Failed to send num_levels.")
		exit(1)
	
	
	'''
		Enable motors
	'''
	# Re-enable Arduino when this is done.
	print("Enabling motors")
	try:
		bus.write_byte(arduino_addr, ord('e'))  # 'e' for enable motors!
	except OSError:
		print("OSError: Failed to write to specified peripheral")
		exit(1)
	
	
	'''
		Picture loop
	'''
	while True:
	
		my_move = ord(input("\nGive direction: "))
			# ord(str) gets the ascii value of str, so we can send the
			#	input as an int over I2C.
		
		if my_move == ord('e') or my_move == ord('q'):
			print("'" + str(my_move) + "'")
			try:
				bus.write_byte(arduino_addr, my_move)  # 'e' for enable motors!
			except OSError:
				print("OSError: Failed to write to specified peripheral")
		else:
			scoot_camera(bus, my_move, arduino_addr)
			print("Taking picture")
			#sleep(2)  # Take a picture
		

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
		