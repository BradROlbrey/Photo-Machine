
# https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all#i2c-on-pi


from sys import exit
import smbus
from time import sleep
from picamera import PiCamera
from datetime import datetime


# Initialize I2C (SMBus)
bus = smbus.SMBus(1)
arduino_addr = 0x7f


def main():

	camera = PiCamera()
	camera.resolution = (3280, 2464)
	camera.vflip = True
	camera.start_preview()
	
	
	'''
		Disable motors
	'''
	# Disable the motors so we can manually manipulate the machine.
	print("Disabling motors")
	for i in range(1):
		write_byte(ord('q'), "OSError: Failed to disable motor drivers", 1)
			# For some reason, sending first 'q' is unreliable. Delay does not fix,
			#	so sending multiple q's with small delays.
			# Oddly enough, the Arduino's receive function recognizes the 'q', but
			#	then it never goes into the switch statement.
			# Sending 'q' or other letters doesn't appear to be the slightest problem
			#	later on, thankfully.
	
	# try:
	# 	for i in range(5):
	# 		bus.write_byte(arduino_addr, ord('q'))
	# 		sleep(.1)
	# except OSError:
	#	print("OSError: Failed to disable motor drivers")
	#	exit(1)
	
	print("Make sure the Arm is rotated such that the motor wires twist properly and Slider is at bottom-most position")
	
	
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
		#sleep(.1)  # Give some time for the Arduino to process back-to-back I2C comms.
	except OSError:
		print("OSError: Failed to send num_levels.")
		exit(1)
	
	
	'''
		Enable motors
	'''
	# Re-enable stepper drivers when this is done.
	print("Enabling motors")
	write_byte(ord('e'), "OSError: Failed to enable motor drivers", 1)

	# try:
	# 	wait_for_processing()
	# 	bus.write_byte(arduino_addr, ord('e'))  # 'e' for enable motors!
	# 	#sleep(.1)
	# except OSError:
	# 	print()
	# 	exit(1)
	
	
	'''
		Picture loop
	'''
	
	# Take pictures as we go up, return down, rotate, repeat.
	# Returning down before rotating prevents the ribbon cable from bunching up and tangling
	#	with other things.
	# Also makes it much easier to return the camera to the start position.

	# for each rotation
	for i in range(num_photos_per_rev):
		# for each level
		for j in range(num_levels):

			time = "{:0>2}h{:0>2}m{:0>2}s".format(
				datetime.now().time().hour,
				datetime.now().time().minute,
				datetime.now().time().second
			)
			total_pics = i * num_levels + j + 1
			name = 'test-{}_{:0>2}-{:0>2}_{:0>3}.jpg'.format(time, i+1, j+1, total_pics)

			#print("Waiting for move in loops")
			wait_for_moving()
			sleep(.25)
			print("Taking picture", name)
			camera.capture(name)
			sleep(.25)
			
			# Scoot up as we take photos
			# ord(str) gets the ascii value of str, so we can send the
			#	input as an int over I2C.
			if j != num_levels-1:
				write_byte(ord('w'))
			# else continue
			# Need the, say, 5 photos. But only want 4 moves. So skip move on last loop.

		# Return down all the way once we're done with this longitude and rotate camera one move.
		# Need the, say, 3 levels. But only want 2 move rotations. So skip rotation on last loop.
		# Will handle last move down outside of loop.
		if i != num_photos_per_rev-1:
			write_byte(ord('s'))
			wait_for_moving()
			write_byte(ord('d'))
	
	# Rotate the camera back to the beginning and return down all the way.
	write_byte(ord('s'))
	wait_for_moving()
	write_byte(ord('a'))
	wait_for_moving()

	print("Finished! Disabling motor drivers")
	write_byte(ord('q'), "OSError: Failed to disable motor drivers")

	# try:
	# 	bus.write_byte(arduino_addr, ord('q'))
	# except OSError:
	# 	print("OSError: Failed to disable motor drivers")
		

def write_byte(inp_byte, custom_message="OSError: Failed to write byte to specified peripheral", exit_status=0):
	# Everytime we write a byte, we must
	# 	do error handling and
	# 	wait for arduino to be ready to accept more input.
	
	wait_for_processing()

	try:
		print("Writing byte:", inp_byte)
		bus.write_byte(arduino_addr, inp_byte)
	except OSError:
		print(custom_message)

		if exit_status:
			exit(exit_status)


# def scoot_camera(direction):
# 	#print("Requesting move")
# 	try:
# 		bus.write_byte(arduino_addr, direction)
# 	except OSError:
# 		print("OSError: Failed to write to specified peripheral")
# 	sleep(.1)  # Give the Arduino some tiny breathing room to receive the input and
# 	# set its status (int moving) before requesting it.
# 	# Doesn't matter if it finished moving before this is done. Probably.

	
def wait_for_moving():
	while True:
		print("Waiting for move in function:", end=' ')
		moving = read_bytes_from_arduino(0)[0]
		print("Moving:", moving)
		if moving == 0:
			return
		#print("Waiting for Arduino to finish moving", arduino_status)
		sleep(.5)
		
def wait_for_processing():
	while True:
		processing = read_bytes_from_arduino(1)[1]
		print("Processing:", processing)
		if processing == 0:
			return
		sleep(.1)

def read_bytes_from_arduino(which_byte):
	buffer = None
	for _ in range(10):  # Try 10 times, if it doesn't work, exit.
		try:
			buffer = bus.read_i2c_block_data(arduino_addr, which_byte, 2)
			return buffer
		except OSError:
			print("OSError: Failed to read from specified peripheral")
			sleep(.5)
	print("Failed 10 times trying to read from Arduino, something wrong, exiting program.")


if __name__ == '__main__':
	main()
		