/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include "evgenii.h"
#include <stdlib.h>


/*******************************************************weekly assignments***************************************/

//week 3 Exercise 1

void week3_1_evg(void)
{
    
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    IR_Start();
    IR_wait();
    motor_forward(125,2750);     // moving forward from starting point
    
    tankTREvg(255, 103);           // 1st turning right 
    motor_forward(125,2350);    // forward on horisontal line
    tankTREvg(255, 103);           // 2nd turn
    motor_forward(125,2550);
    tankTREvg(255, 103);           // 2nd turn
    tankTurnEvg(164,135,1780);
    motor_forward(125,570);     // moving forward from starting point
    motor_forward(0,0);         // stop motors
    motor_stop();               // disable motor controller
    
    progEndEvg(100);
}

//week 3 Exercise 2

void week3_2_evg(void) 
{
    printf("Press IR send to start and button on PSoC to stop");
    Ultra_Start();              // Ultra Sonic Start function
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    
    IR_Start();
    IR_wait();
    while(true) {
        motor_forward(150, 50);
        int d = Ultra_GetDistance();
        // Print the detected distance (centimeters)
        printf("distance = %d\r\n", d);
        vTaskDelay(100);
        if(d < 10){
            motor_forward(0,10);         
            motor_backward(100, 150);
            motor_turn(0, 150, 462);
        }
        if(!SW1_Read()){
             break;
        }
    }
    motor_forward(0,0);         
    motor_stop();

}

//week 3 Exercise 3

void week3_3_evg(void) 
{
    printf("Press IR send to start and button on PSoC to stop");
    Ultra_Start();              // Ultra Sonic Start function
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    IR_Start();
    IR_wait();
    while(true) {
        motor_forward(125, 50);
        int d = Ultra_GetDistance();
        // Print the detected distance (centimeters)
      //  printf("distance = %d\r\n", d);
        vTaskDelay(200);
       int turn = randomEvg(103, 309);
        if(d < 11){
            motor_forward(0,10);         
            motor_backward(100, 150);
            if (turn % 2 == 1) {
               tankTLEvg(255, turn);
               printf("\nTurn value is %i so I turn left\n", turn);
            } else {
               tankTREvg(255, turn);
               printf("\nTurn value is %i so I turn right\n", turn);
            }
        }
        if(!SW1_Read()){
             break;
        }
    }
    motor_forward(0,0);         
    motor_stop();

}

//week 4 Exercise 1

void week4_1_evg(void) 
{
    uint32_t count = 0;
    struct sensors_ dig;
    reflectance_start(); 
    IR_Start();
    motor_start();  
    motor_forward(0,0);
    reflectance_set_threshold(14000, 14000, 11000, 11000, 14000, 14000);  
    
    while(SW1_Read());
      
    reflectance_digital(&dig);
    while(count<5) 
    {
        driveForward();
        count++;
        printf("one loop, %i lines passed by\n", count);
        if(count == 1){
            motor_forward(0,0);
            IR_wait();
        }
    }
    printf("Number of lines is %i\n", count);
    motor_forward(0,0);         
    motor_stop();
}


//week 4 Exercise 2

void week4_2_evg(void) 
{
    struct sensors_ dig;
    reflectance_start(); 
    IR_Start();
    motor_start();  
    motor_forward(0,0);
    reflectance_set_threshold(11000, 11000, 11000, 11000, 11000, 11000);  
    IR_wait();
    //while(SW1_Read());
    reflectance_digital(&dig); 
    driveForward();
        IR_wait();  
        while(dig.L3 == 0 && dig.R3 == 0)
        {
            motor_forward(50,0);
            reflectance_digital(&dig); 
            
            if(dig.R2 == 1)
            {   
                tankTurnEvg(150,0,0);
                reflectance_digital(&dig); 
            }
            if(dig.L2 == 1)
            {   
                tankTurnEvg(0,200,0);
                reflectance_digital(&dig); 
            }
        }
    printf("end of loop");
    motor_forward(0,0);         
    motor_stop();
}
//week 4 Exercise 2

void week4_3_evg(void) 
{
    uint32_t count = 0;
    struct sensors_ dig;
    reflectance_start(); 
    IR_Start();
    motor_start();  
    motor_forward(0,0);
    reflectance_set_threshold(11000, 11000, 11000, 11000, 11000, 11000);  
    IR_wait();
   // while(SW1_Read());
    reflectance_digital(&dig);
    while(count<5) 
    {
        driveForward();
        count++;
        printf("one loop, %i lines passed by\n", count);
        
        // wait on first line
        if(count == 1){
            motor_forward(0,0);
            IR_wait();
          
        } else if(count == 2) {
           // motor_forward(0,0);
           // IR_wait();
            printf("count is two!\n");
         //turn left on second line  
            reflectance_digital(&dig);
          while(!(dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0)  ) {
                motor_turn(4,250,50);
                reflectance_digital(&dig);
        }
        
       // printf("L3 = %d, L2 = %d, L1 = %d, R1= %d, R2= %d, R3= %d\n", dig.L3 , dig.L2 , dig.L1 , dig.R1 ,dig.R2 , dig.R3);
        //motor_forward(0,0);    
        //IR_wait();
        } else if(count == 3) {
          //  motor_forward(0,0);
            //IR_wait();
            
           //turn right on third line
            reflectance_digital(&dig);
          while(!(dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0) ) {
            //printf("while -3 \n");
                motor_turn(230,10,70);
                reflectance_digital(&dig);
        }
        } else if(count == 4) {
           // motor_forward(0,0);
            //IR_wait();
            
           //turn right on fourth line
            reflectance_digital(&dig);
          while(!(dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0)) {
           
                motor_turn(230,10,50);
                reflectance_digital(&dig);
        }
        } 
    }
    
    printf("end of loop");
    motor_forward(0,0);         
    motor_stop();
}




void driveForward(void){
    struct sensors_ dig;
    reflectance_start(); 
    reflectance_digital(&dig);
    //drives forward when on black
    while(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)
    {
        motor_forward(100,0);
        reflectance_digital(&dig); 
    }
    //drives forward when on white
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1))
    {
        motor_forward(100,0);
        reflectance_digital(&dig); 
    }
    motor_forward(0,0);
}





void progEndEvg(uint32_t delay) {
    bool led = false;
    while(true){
     BatteryLed_Write(led^=1);
     vTaskDelay(delay);   
    }
}
void tankTLEvg(uint8_t speed, uint32_t delay) {
    SetMotors(1, 0, speed, speed, delay);
}

void tankTREvg(uint8_t speed, uint32_t delay) {
    SetMotors(0, 1, speed, speed, delay);
}

int randomEvg(int min, int max) {
    srand(xTaskGetTickCount());
    int random = (rand() % (max - min + 1)) + min;
    return random;
}



void tankTurnEvg(uint8 l_speed, uint8 r_speed, uint32 delay){
    SetMotors(0,0, l_speed, r_speed, delay);
}






/* [] END OF FILE */
