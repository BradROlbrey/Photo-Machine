
# https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all#i2c-on-pi


from sys import exit
import smbus
from time import sleep
		
		
def main():

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
			# Oddly enough, the Arduino's receive function recognizes the 'q', but
			#	then it never goes into the switch statement.
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
		sleep(.1)  # Give some time for the Arduino to process back-to-back I2C comms.
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
		sleep(.1)
	except OSError:
		print("OSError: Failed to enable motor drivers")
		exit(1)
	
	
	'''
		Picture loop
	'''
	# We assume we start at the bottom, and only need to go up.
	# We will start by going clockwise.
	rotation_dir = 'd'
	
	# for each level
	for i in range(num_levels):
		# for each picture in the rotation
		for j in range(num_photos_per_rev):
			print("Taking picture")
			sleep(1)  # Take a picture
			
			if j != num_photos_per_rev-1:
				scoot_camera(bus, ord(rotation_dir), arduino_addr)
			# else continue
			# Need the, say, 5 photos. But only want 4 moves. So skip move on last loop.

		# Move the camera up after a full rotation.
		# ord(str) gets the ascii value of str, so we can send the
		#	input as an int over I2C.
		if i != num_levels-1:
			scoot_camera(bus, ord('w'), arduino_addr)
		# Need the, say, 3 levels. But only want 2 moves. So skip move on last loop.
			
		# Change the direction.
		if rotation_dir == 'd':
			rotation_dir = 'a'
		elif rotation_dir == 'a':
			rotation_dir = 'd'
	
	print("Finished! Disabling motor drivers")
	try:
		bus.write_byte(arduino_addr, ord('q'))
	except OSError:
		print("OSError: Failed disable motor drivers")
		

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
		