'''Author: Thomas W. Talbot
   Last Date Modified: 11/22/2021
   Purpose: The purpose of this program is to read data from the mbed serial
   port '/dev/ttyACM0'. If the device sends a one then begin backup camera
   capture
'''

#!/usr/bin/env python
import time
import serial
from picamera import PiCamera
from time import sleep
camera = PiCamera()
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
    if x == b'1':
        print("Made it into the if statement")
        #record five seconds of video from the PiCamera
        camera.start_preview()
        camera.start_recording('/home/pi/Desktop/video.h264')
        sleep(5)
        camera.stop_recording()
        camera.stop_preview()
        
    
