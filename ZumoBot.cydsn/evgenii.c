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
        vTaskDelay(300);
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



/*   USEFUL functions

void week3_1(void)
{
    
    motor_start();              // enable motor controller
    IR_Start();
    motor_forward(0,0);         // set speed to zero to stop motors
   // while(SW1_Read() != PRESSED);
    IR_wait();
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    motor_forward(125,2750);     // moving forward from starting point
    tankTR(255, 103);           // 1st turning right 
    
    motor_forward(125,2350);    // forward on horisontal line
    tankTR(255, 103);           // 2nd turn
    motor_forward(125,2550);
    tankTR(255, 103);           // 2nd turn
    tankTurn(164,135,1780);
    motor_forward(125,570);     // moving forward from starting point
    
    motor_forward(0,0);         // stop motors
    motor_stop();               // disable motor controller
    
    progEnd(100);
}
*/



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
