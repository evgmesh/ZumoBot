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
#include <stdint.h>

#define PRESSED 0
#define RELEASED 1

//WEEK 3 ASSIGNMENT 1 *****************************************************************************************
void aneta_w3a1(void){
    
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


//WEEK 3 ASSIGNMENT 2 *****************************************************************************************
void aneta_w3a2(void){
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


//WEEK 3 ASSIGNMENT 3 *****************************************************************************************
void aneta_w3a3(void){
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


//WEEK 4 ASSIGNMENT 1 *****************************************************************************************
void aneta_w4a1(void){
    bool wait=1;
    
    printf("ASSIGNMENT 1\n");
    motor_start();
    IR_Start();
    
    while(wait)
    {
        vTaskDelay(100);
        if (!SW1_Read()){               //if button is pressed
            wait=0;                     //then end the loop
            motor_forward(100,200);     // and move forward to the first line
        }
    }
    
    motor_forward(0,0);                 //stop
    IR_wait();                          //and wait for IR signal
    
    motor_forward(122,1000);            //then move to the last line
    printf("My job is done \n");

    motor_stop();

}

//WEEK 4 ASSIGNMENT 2 *****************************************************************************************
void aneta_w4a2(void){
    bool wait=1;
    
    printf("ASSIGNMENT 2\n");
    motor_start();
    IR_Start();
    
    while(wait)
    {
        vTaskDelay(100);
        if (!SW1_Read()){               //if button is pressed
            wait=0;                     //then end the loop
            motor_forward(100,180);     //and move forward to the first line
        }
    }
    
    motor_forward(0,0);                 //stop
    IR_wait();                          //and wait for IR signal
                                        //then move to the last line
    motor_forward(100,280); 
    motor_turn(92,40,700);
    motor_forward(100,245); 
    motor_turn(36,100,1380);
    motor_forward(100,288); 
    
    
    printf("My job is done \n");

    motor_stop();

}

//WEEK 4 ASSIGNMENT 3 *****************************************************************************************
void aneta_w4a3(void){
    
    printf("ASSIGNMENT 3\n");
    motor_start();
    

    motor_stop();

}

//WEEK 5 ASSIGNMENT 1 *****************************************************************************************
void aneta_w5a1(void){
    int btn1Time = 0; 
    int btn2Time = 0;
    int time;
    
    printf("ASSIGNMENT 1\n");
    motor_start();
    
        while (SW1_Read()== RELEASED){      
            vTaskDelay(0);
        }
        btn1Time = xTaskGetTickCount();         //when button is pressed for the first time
        
        
    while (true){    
        
        while (SW1_Read() == RELEASED){         //wait for the button to be pressed
            vTaskDelay(0);
        }
        btn2Time = xTaskGetTickCount();         //get time of the press
        
        while (SW1_Read() == PRESSED){          //wait for the button to be released
            vTaskDelay(0);        
        }
        
        time = btn2Time - btn1Time;             //find the time betweeen presses
        print_mqtt("zumo6/button", "Time: %d ms\n", time );
        btn1Time = btn2Time;
        
    }
    motor_stop();

}

//WEEK 5 ASSIGNMENT 2 *****************************************************************************************
void aneta_w5a2(void){
    
    int obsD;           // how far the obstacle is
    int myD = 10;       // how far should obstacle be when robot should turn
    int delay = 290;    // delay for motors; if delay is 290 and speed of motors is X, then robot turns X degrees (left/right)
    int degreeTurn;     // how many degrees robot turns
    bool move = true;   // if true robot keeps moving
    bool turnRight;     // if true robot turns right, else turns left
    
    printf("ASSIGNMENT 2\n");
    motor_start();
    Ultra_Start();

    while(move){
        motor_forward(50, 100);
        obsD = Ultra_GetDistance();
       
        if(obsD < myD){
            motor_forward(0,0);         
            motor_backward(50,100);
            turnRight = rand() % 2;  
            
            if (turnRight){
                print_mqtt("zumo6/turn", "Right turn\n" );
                aneta_tankTurnRight(90,delay);              //90 deg turn right
            } else {
                print_mqtt("zumo6/turn", "Left turn\n" );
                aneta_tankTurnLeft(90,delay);               //90 deg turn left
            }
            
        }
        
        if(!SW1_Read()){        //robot stops moving when the user button is pressed
             move = false;
        }
    
    }


    motor_stop();

}

//WEEK 5 ASSIGNMENT 3 *****************************************************************************************
void aneta_w5a3(void){
    
    printf("ASSIGNMENT 3\n");
    motor_start();
    

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
