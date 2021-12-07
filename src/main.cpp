/*******************************************************************************
* Author: Thomas W.  Talbot 
* Last Date Modified: 11/20/21
* Purpose: This program runs on the mbed microcontroller and continually checks
* the Adafrtuit BLE module for drive commands. 
*******************************************************************************/
//TODO: Set up ticker to play audio samples.

#include "mbed.h"
#include "rtos.h"
#include "VL53L0X.h"
#include "Motor.h"

#define brake_value 0.1
#define WARN_COLOR 0x987F00
#define INDICATOR_COLOR 0xFF0000
#define BACKUP_COLOR 0xFFFFFF
/* RGB LEDs for blindspot detection and turn signal */
PwmOut R_left_light(p25);
PwmOut G_left_light(p26);
DigitalOut B_left_light(p19);

PwmOut R_right_light(p24);
PwmOut G_right_light(p23);
DigitalOut B_right_light(p20);
/* ---------------------- */

/* Audio output */
#include "horn.h"
#define SAMPLE_FREQ 16000
AnalogOut audio(p18);
/* ---------------------- */

/* ToF sensors */ 
DevI2C i2c(p9, p10);
DigitalOut  shdn1(p11), shdn2(p12);
VL53L0X     ld1(&i2c, &shdn1, NC), ld2(&i2c, &shdn2, NC); 
/* ---------------------- */

/* Bluetooth */

Serial blue(p28, p27); 
/* ---------------------- */

/* Motors */
Motor right(p21, p5, p6); 
Motor left(p22, p7, p8); 
/* ---------------------- */

/* Debug */
Serial pc(USBTX, USBRX); 
DigitalOut led1(LED1); 
DigitalOut led2(LED2);
/* ---------------------- */


volatile float right_speed =  0; //absolute value 0-1
volatile float left_speed = 0; //absolute value 0-1 
volatile bool forward = false; //car starts in reverse gear


volatile bool left_detect = 0;
volatile bool right_detect = 0;
volatile bool left_indicate = 0;
volatile bool right_indicate = 0;
volatile bool blink = 0;
Mutex rgb_mutex;

void set_rgb(int color, bool direction) {
    float color_r = (float)((color & 0xFF0000) >> 16) * 0.00390625;
    float color_g = (float)((color & 0x00FF00) >> 8) * 0.00390625;
    if (direction == 0) {
        R_left_light = color_r;
        G_left_light = color_g;
        if ((color & 0xFF) == 0xFF) {
            B_left_light = 1;
        }
        else {
            B_left_light = 0;
        }
    
    }
    else if (direction == 1) {
        R_right_light = color_r; 
        G_right_light = color_g;
        if ((color & 0xFF) == 0xFF) {
            B_right_light = 1;
        }
        else {
            B_right_light = 0;
        }
    }
}   


volatile bool right_blindspot = 0;
volatile bool left_blindspot = 0;

void indicator(void const *args) {
    while(1) {
        if (left_indicate) {
            set_rgb(INDICATOR_COLOR,0);
            Thread::wait(500);
            set_rgb(0,0);
            Thread::wait(500); 
        }
        else if (right_indicate) {
            set_rgb(INDICATOR_COLOR,1);
            Thread::wait(500);
            set_rgb(0,1);
            Thread::wait(500);
        }
        else if (left_blindspot | right_blindspot) {
            if (right_blindspot) {
                set_rgb(WARN_COLOR,1);
            }
            if (left_blindspot) {
                set_rgb(WARN_COLOR,0);
            }
        }
        else {
            set_rgb(0,0);
            set_rgb(0,1);
        }
    }
}

volatile bool brakes_hit = 0;

void bluetooth(void const *args) {
    while(1) 
    {
        if(blue.readable() && blue.getc() == '!')
        {
            pc.printf("Bluetooth is readable\n\r"); 
            if(blue.getc() == 'B')
            {
                
                //recieved a button data packet
                char bnum = blue.getc(); 
                char bhit = blue.getc(); 
                if (blue.getc()==char(~('!' + 'B' + bnum + bhit))) { //checksum OK?
                    switch(bnum)
                    {
                        case '3': 
                            if(bhit == '1')
                            {
                                //apply brakes, decreasing the speed by 0.1 
                                //brakes cannot change the direction 
                                brakes_hit = 1;
                                if(forward)
                                {
                                    //apply brakes to forward motion
                                    if(right_speed>0.01)
                                    {
                                        right_speed = right_speed  - brake_value; 
                                    }
                                    if(left_speed > 0.01)
                                    {
                                        left_speed = left_speed - brake_value; 
                                    }
                                }
                                else
                                {
                                    //apply brakes in reverse motion 
                                    
                                    if(right_speed < -0.01)
                                    {
                                        right_speed = right_speed + brake_value; 
                                    }
                                    if(left_speed < -0.01)
                                    {
                                        left_speed = left_speed +brake_value; 
                                    }
                            
                                } //end if-else block on forward
                            }//end if-else block on bhit
                            else {
                                set_rgb(0,0);
                                set_rgb(0,1);
                            }
                            break;  
                                    
                        case '5':
                            //Up arrow - increase forward speed 
                            if(bhit == '1')
                            {
                                pc.printf("up arrow hit\n\r"); 
                                if(!forward)
                                {
                                    //just switched into forward gear
                                    //the car should be stopped
                                    forward =true; 
                                    right_speed = 0.0; 
                                    left_speed = 0.0; 
                                }
                                else
                                {
                                    if(right_speed <1.0)
                                    {
                                        //pc.printf("adding to the right speed\n\r"); 
                                        right_speed = right_speed + 0.1; 
                                        //pc.printf("%f\n\r", right_speed); 
                                    }
                                    if(left_speed <1.0)
                                    {
                                        //pc.printf("Adding to the left speed\n\r"); 
                                        left_speed = left_speed + 0.1; 
                                        //pc.printf("%f\n\r", left_speed);
                                    }
                                }// end if-else block 
                            }//end hit if block 
                            break; 
                        case '6':
                             if(bhit == '1')
                             {
                                 pc.printf("down arrow hit\n\r");
                                 
                                 //Down arrow - throw the car into reverse and increase 
                                 if(forward) 
                                 {
                                     //just moved into reverse gear 
                                     //the car should be stopped
                                     forward = false; 
                                     right_speed = 0.0; 
                                     left_speed = 0.0; 
                                 }
                                 else
                                 {
                                     if(right_speed > -1.0)
                                     {
                                         right_speed = right_speed -0.1; 
                                     } 
                                     if(left_speed > -1.0) 
                                     {
                                         left_speed = left_speed - 0.1; 
                                     } 
                                }//end the if-else block 
                            }//end hit if block 
                            break; 
                        case '7': 
                            //left arrow 
                            if(bhit == '1') 
                            {   
                                left_indicate = 1;
                                float old_right_speed = right_speed; 
                                float old_left_speed = left_speed; 
                                //Only apply a left turn for 2 seconds  
                                left_speed = -1*old_right_speed;  
                                Thread::wait(2000); 
                                //reset the original speed  
                                right_speed = old_right_speed; 
                                left_speed = old_left_speed; 
                            }//end bhit if 
                            else{
                                left_indicate = 0;
                            }
                            break; 
                            
                        case '8':
                            //right arrow 
                            if(bhit == '1') 
                            {
                                right_indicate = 1;
                                float old_right = right_speed; 
                                float old_left = left_speed; 
                                //set right wheel speed to - left speed 
                                right_speed = -1*old_left; 
                                Thread::wait(2000); 
                                //reset the speeds 
                                right_speed = old_right; 
                                left_speed = old_left; 
                            }//end bhit if
                            else{
                                right_indicate = 0;
                            }            
                            break; 
                            
                        default:
                            break; 
                    } //end switch statement 
                }//end if statement for checksum
            }//end if statement for button
        }//end  if statement for readable 
        //Thread::wait(100); 
    }//end while loop 
}

void speed_control(void const *args)
{  
    while(1)
    {
        right.speed(right_speed); 
        left.speed(left_speed); 
        
        Thread::wait(200); 
    }  
}

#define threshold_mm 300
void blind_spot(void const *args) {
    ld1.VL53L0X_off();
    ld2.VL53L0X_off();
    Thread::wait(0.1);
    ld1.init_sensor(0x30);
    Thread::wait(0.1);
    ld2.init_sensor(0x50);
    uint32_t distance1 = 0;
    uint32_t distance2 = 0;
    int status;
    int status2;
    while (1) {
                
        status = ld1.get_distance(&distance1);
        status2 = ld2.get_distance(&distance2); 
        while(status != VL53L0X_ERROR_NONE)
        {   
            ld1.get_distance(&distance1); 
        }
        Thread::wait(0.1); 
        while(status2 != VL53L0X_ERROR_NONE)
        {
            ld2.get_distance(&distance2); 
        }
        
        if (distance1 < threshold_mm) { 
            right_blindspot = 1;
            led1 = 1;
        }
        else { 
            right_blindspot = 0;
            led1 = 0;
        }
        
        if (distance2 < threshold_mm) {
            left_blindspot = 1;
            led2 = 1;
        }
        else {
            left_blindspot = 0;
            led2 = 0;
        }
        Thread::wait(0.2);
        
    }
}

int main() {
    Thread t1(indicator);
    Thread t2(bluetooth);
    Thread t3(speed_control);
    Thread t4(blind_spot);
    while(1) {
        Thread::wait(1000);
    }
}