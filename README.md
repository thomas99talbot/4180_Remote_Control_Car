# Blindspot Monitoring and Backup Camera
## Team Members
[Thomas Talbot](https://github.com/thomas99talbot)  
[Kelly Helmut Lord](https://github.com/hlord2000)
## Introduction
The purpose of the following project was to implement common safety features that are present on modern cars.  The car is controlled by using the Adafruit BLE UART Friend and its associated app.  Blindspot monitoring is accomplished by using two VL53L0X ToF distance sensors which notify the driver with two RGB LED indicators on each side.  A Raspberry Pi 4B is used to control the Raspberry Pi Camera Module serving to provide vision behind the car.  The car is equipped with a horn, similar to a Ford Model A Sport Coupe's klaxon.    
## Bill of Materials
* [Mbed LPC1768](https://os.mbed.com/platforms/mbed-LPC1768/)
* [Raspberry Pi 4 Model B](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)
* [ECE 4180 Embedded IoT & Robot Makers Kit for Mbed](http://hamblen.ece.gatech.edu/489X/embkit/)
* [Raspberry Pi Camera Module 2](https://www.raspberrypi.com/products/camera-module-v2/)
* [Adafruit Bluefruit LE UART Friend](https://www.adafruit.com/product/2479)
* [Adafruit VL53L0X Time of Flight Distance Sensor](https://www.adafruit.com/product/3317)
* [SparkFun Mono Audio Amp Breakout - TPA2005D1](https://www.sparkfun.com/products/11044)
* [Speaker](https://www.sparkfun.com/products/11089)
* [SparkFun Motor Driver - Dual TB6612FNG](https://www.sparkfun.com/products/14451)
* [RGB LED (x2)](https://www.sparkfun.com/products/16911)
* [DC Barrel Jack](https://www.sparkfun.com/products/119)
## Pinouts
![Motor Driver](https://github.com/thomas99talbot/4180_Remote_Control_Car/blob/mbed_Helmut/DualHBridge.png)
| **Mbed** | **Dual TB6612FNG Motor Driver** |
|------|-----------------------------|
| Vin  | VM                          |
| Vout | VCC                         |
| GND  | GND                         |
| P21  | PWMA                        |
| P22  | PWMB                        |
| P5   | AI1                         |
| P6   | AI2                         |
| P7   | BI1                         |
| P8   | BI2                         |
| Vout | STBY                        |