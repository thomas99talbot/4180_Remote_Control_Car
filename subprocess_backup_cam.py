'''Author: Thomas W. Talbot
   Last Date Modified: 12/09/2021
   Purpose: The purpose of this program is to read data from the mbed serial
   port '/dev/ttyACM0'. If the device sends a 'b'1' then begin backup camera
   capture script "video_stream.sh". After 32 seconds, throw a TimeoutExpired exception
   on the video_streamsh subprocess and read a new character from the mbed.
'''
import time
import serial
import subprocess

ser = serial.Serial(

	port = '/dev/ttyACM0',
	baudrate = 9600, 
	parity = serial.PARITY_NONE, 
	stopbits = serial.STOPBITS_ONE, 
	bytesize = serial.EIGHTBITS, 
	timeout =1
)
while 1:
    
    x = ser.readline()
    print(x)
    print("Testing")
    if x == b'1': #if the car is in reverse 
        #Start the backup camera for 30 seconds
        pro = subprocess.Popen("/home/pi/Desktop/4180Code/video_stream.sh")
        while 1:
            if not pro.returncode:
                #The subprocess has not returned yet
                try:
                    pro.wait(timeout=32) # This throws a TimeoutExpired error on the video stream
                except subprocess.TimeoutExpired:
                    print("Timeout expired error occurred")
                    pro.kill()
                    break #leave inner while loop to read more characters from the mbed
        
    
                