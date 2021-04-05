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
#include <stdlib.h>
#include "aneta.h"
#include "evgenii.h"
#include "Dongbin.h"


/************************ week3 Ex.1 ************************************/

void week3_1_DB(void){
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
   
    while(SW1_Read());
    BatteryLed_Write(true);
   
    vTaskDelay(500);
   
    BatteryLed_Write(false);
    motor_forward(125,2800);     //moving forward
    tankTR_DB(255,103);             //turn left
    motor_forward(125,2350);    //continue moving forward
    tankTR_DB(255,103);
    motor_forward(125,2500);
    tankTR_DB(255,123);
    
    int i = 0;
    while(i < 22){
        motor_forward(125,85);
        softTR_DB(1+i);
        i++;
        if(i == 22){
          softTR_DB(6);   
        }
    }
    motor_forward(125,728);
    motor_forward(0,0);         // stop motors
    motor_stop();               // disable motor controller
    
    progEnd_DB(500);
}

/************************ week3 Ex.2 ************************************/

void aneta_w1a2(void){
    int obsD;           // how far the obstacle is
    int myD = 10;       // how far should obstacle be when robot should turn
    bool move = true;   // if true robot keeps moving
    
    printf("ASSIGNMENT 2\n");
    Ultra_Start();
    motor_start();
    
    while(move){
        motor_forward(50, 100);
        obsD = Ultra_GetDistance();
       
        if(obsD < myD){
            motor_forward(0,0);         
            motor_backward(50,100);
            motor_turn(0,255,320);
        }
        
        if(!SW1_Read()){        //robot stops moving when button is pressed
            move = false;
        }
    
    }

    motor_stop();

}

/************************ week3 Ex.3 ************************************/

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










































/*********************************** DB Custom Functions **************************/

void progEnd_DB(uint32_t delay){
    bool led = false;
    while(true){
        BatteryLed_Write(led^=1);
        vTaskDelay(delay);
    }
}
void tankTL_DB(uint8_t speed, uint32_t delay){
    SetMotors(1,0,speed,speed,delay);
}
void tankTR_DB(uint8_t speed, uint32_t delay){
    SetMotors(0,1,speed,speed,delay);
}
void softTR_DB(uint32_t delay){
    SetMotors(0,1,80,80,delay); 
}



/******************************* Aneta Custom Functions *****************************/

void aneta_tankTurnRight(uint8_t speed, uint32_t delay){
    SetMotors(0, 1, speed, speed, delay);
}

void aneta_tankTurnLeft(uint8_t speed, uint32_t delay){
    SetMotors(1, 0, speed, speed, delay);
}

/******************************** Evgenii Custom Functions ****************************/

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

void tankTurnEvg(uint8_t l_speed, uint8_t r_speed, uint32_t delay){
    SetMotors(0,0, l_speed, r_speed, delay);
}