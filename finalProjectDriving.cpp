/*******************************************************************************
* Author: Thomas W.  Talbot 
* Last Date Modified: 11/22/21
* Purpose: This program runs on the mbed microcontroller and continually checks
* the Adafrtuit BLE module for drive commands. 
*******************************************************************************/


#include "mbed.h"
#include "rtos.h"
#include "Motor.h"
#define brake_value 0.1


Serial blue(p28, p27); 
//Serial pc(USBTX, USBRX); 
RawSerial pi(USBTX, USBRX); 
Motor right(p21, p5, p6); 
Motor left(p22, p7, p8); 
DigitalOut led1(LED1); 
DigitalOut led2(LED2);

volatile float right_speed =  0.0; //absolute value 0-1
volatile float left_speed = 0.0; //absolute value 0-1 
volatile bool forward = true; //car starts in reverse gear

/*******************************************************************************
* speed_control - The purpose of this function is to set the speed on the left 
* and right DC car motors twice a second. 
*******************************************************************************/
void speed_control(void)
{  
    while(1)
    {
        //pc.printf("Setting the right_speed to %f\r\n", right_speed); 
        //pc.printf("Setting the left_speed to %f\r\n", left_speed); 
        right.speed(right_speed); 
        left.speed(left_speed); 
        
        Thread::wait(200); 
    }  
}

/*******************************************************************************
* backup_camera - The purpose of this function is to check if the backup camera 
* should come on every 5 seconds. 
*******************************************************************************/    

void backup_camera(void)
{
    while(1)
    {
        while(!pi.writeable()){}
        led2 = !led2;
        if(!forward)
        {
            pi.putc('1'); 
        }    
        Thread::wait(2500); 
    }//end main while loop 
}
            
int main() {
    Thread t1(speed_control); 
    pi.baud(9600); 
    Thread t2(backup_camera); 
    char bnum =0; 
    char bhit =0; 
    while(1) 
    {
        if(blue.readable() && blue.getc() == '!')
        {
            //pc.printf("Bluetooth is readable\n\r"); 
            if(blue.getc() == 'B')
            {
                
                //recieved a button data packet
                bnum = blue.getc(); 
                bhit = blue.getc(); 
                if (blue.getc()==char(~('!' + 'B' + bnum + bhit))) { //checksum OK?
                    switch(bnum)
                    {
                        case '3': 
                            if(bhit == '1')
                            {
                                //apply brakes, decreasing the speed by 0.1 
                                //brakes cannot change the direction 
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
                            break;  
                                    
                        case '5':
                            //Up arrow - increase forward speed 
                            if(bhit == '1')
                            {
                                //pc.printf("up arrow hit\n\r"); 
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
                                 //pc.printf("down arrow hit\n\r");
                                 
                                 led1 = !led1;
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
                                float old_right_speed = right_speed; 
                                float old_left_speed = left_speed; 
                                //Only apply a left turn for 2 seconds  
                                left_speed = -1*old_right_speed;  
                                Thread::wait(2000); 
                                //reset the original speed  
                                right_speed = old_right_speed; 
                                left_speed = old_left_speed; 
                            }//end bhit if 
                                    
                            break; 
                            
                        case '8':
                            //right arrow 
                            if(bhit == '1') 
                            {
                                float old_right = right_speed; 
                                float old_left = left_speed; 
                                //set right wheel speed to - left speed 
                                right_speed = -1*old_left; 
                                Thread::wait(2000); 
                                //reset the speeds 
                                right_speed = old_right; 
                                left_speed = old_left; 
                            }//end bhit if            
                            break; 
                            
                        default:
                            break; 
                    } //end switch statement 
                }//end if statement for checksum
            }//end if statement for button
        }//end  if statement for readable 
        //Thread::wait(100); 
    }//end while loop 
}//end main











