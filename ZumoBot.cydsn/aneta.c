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
#include "aneta.h"
#include <stdlib.h>

//WEEK 1 ASSIGNMENT 1 *****************************************************************************************
void aneta_w1a1(void){
    
    printf("ASSIGNMENT 1\n");
    motor_start();
    
    motor_forward(100,3350);
    motor_turn(255,0,205); 
    
    motor_forward(100,2600);
    motor_turn(255,0,206);
    
    motor_forward(100,2710);
    motor_turn(255,0,247);
    
    motor_turn(255,230,700);
    motor_turn(100,0,218);
    motor_forward(100,500);
    motor_turn(100,0,30);
    motor_forward(100,700);

    motor_stop();
    
    while(true)
    {
        vTaskDelay(100);
    }
}


//WEEK 1 ASSIGNMENT 2 *****************************************************************************************
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


//WEEK 1 ASSIGNMENT 3 *****************************************************************************************
void aneta_w1a3(void){
    int obsD;           // how far the obstacle is
    int myD = 10;       // how far should obstacle be when robot should turn
    int delay = 290;    // delay for motors; if delay is 290 and speed of motors is X, then robot turns X degrees (left/right)
    int degreeTurn;     // how many degrees robot turns
    bool move = true;   // if true robot keeps moving
    bool turnRight;     // if true robot turns right, else turns left
    
    printf("ASSIGNMENT 3\n");
    Ultra_Start();
    motor_start();
    
    while(move){
        motor_forward(50, 100);
        obsD = Ultra_GetDistance();
       
        if(obsD < myD){
            motor_forward(0,0);         
            motor_backward(50,100);
            turnRight = rand() % 2;
            
            degreeTurn = rand() % 181;  
            
            if (turnRight){
                printf("Right turn %d\n", (degreeTurn+90));
                aneta_tankTurnRight(90,delay);              //90 deg turn
                aneta_tankTurnRight(degreeTurn,delay);      //plus 0 to 180 turn
            } else {
                printf("Left turn %d\n", (degreeTurn+90));
                aneta_tankTurnLeft(90,delay);
                aneta_tankTurnLeft(degreeTurn,delay);
            }
            
        }
        
        if(!SW1_Read()){
             move = false;
        }
    
    }

    motor_stop();

}







//OTHER FUNCTIONS *****************************************************************************************

void aneta_tankTurnRight(uint8_t speed, uint32_t delay){
    SetMotors(0, 1, speed, speed, delay);
}

void aneta_tankTurnLeft(uint8_t speed, uint32_t delay){
    SetMotors(1, 0, speed, speed, delay);
}



/* [] END OF FILE */
