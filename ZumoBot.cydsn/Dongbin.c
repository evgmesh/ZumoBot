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
#include "Dongbin.h"


/************************ week3 Assignment 1 *******************/

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


/******************* week 3 Assignment 2 ***************************/

void week3_2_DB(void){
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    
    while(SW1_Read());
    BatteryLed_Write(true);
    vTaskDelay(500); 
    BatteryLed_Write(false);
    Ultra_Start();              // Ultra Sonic Start function
    int i = 0;
    while(i < 10) {
        int d = Ultra_GetDistance();
        // Print the detected distance (centimeters)
        printf("distance = %d\r\n", d);
        vTaskDelay(300);
        motor_forward(100,200);
        if(d > 0 && d < 20){
           motor_forward(0,0);
           vTaskDelay(300);
           motor_backward(125, 500);
           tankTL120_DB(600);
           i++;
           printf("%d time(s)\n", i);
        }
    }
    motor_forward(0,0);         // stop motors
    motor_stop();               // disable motor controller
    
    progEnd_DB(500);
}

/********************* week 3 Assignment 3 *************************************/
void week3_3_DB(void){
    Ultra_Start();              // Ultra Sonic Start function
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    
    while(SW1_Read());
    BatteryLed_Write(true);
	vTaskDelay(200); 
    BatteryLed_Write(false);
    
    while(true) {
        int d = Ultra_GetDistance();     
        //printf("distance = %d\r\n", d);   // Print the detected distance (centimeters)
        vTaskDelay(300);
        
        motor_forward(80,200);
        if(d < 10){
           motor_forward(0,0);
           vTaskDelay(100);
           motor_backward(125, 1000);
           if(randTurnLR_DB() == 1){
                tankRandTR_DB(randTurnDeg_DB());
                printf("random turn %d\n", randTurnLR_DB());
                printf("random turning delay %d\n", randTurnDeg_DB()); 
            }else{
                tankRandTL_DB(randTurnDeg_DB()); 
                printf("random turn %d\n", randTurnLR_DB());
                printf("random turning delay %d\n", randTurnDeg_DB());
            }
        }
        
        if(!SW1_Read ()){
            break;   
        }
    }
    motor_forward(0,0);         // stop motors
    motor_stop();               // disable motor controller
    
    
    progEnd_DB(500);
}

/****************************** week 4 Assignment 1 ****************************************/
void week4_1_DB(void){
    
    IR_Start();
    onYourMark_DB();    //move the motor to first line
    
    struct sensors_ dig;
    reflectance_start();
    reflectance_set_threshold(13000, 13000, 11000, 11000, 13000, 13000);
    
    IR_wait();
    motor_forward(125,0);
    for(int i = 0; i < 4; i++){
        while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
            reflectance_digital(&dig);
        }
        motor_forward(0,0);
        vTaskDelay(1000);
        while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
            reflectance_digital(&dig);
            motor_forward(125,0);
        }
        printf("loop i = %d\n",i); 
    }
    motor_forward(0,0);
    motor_stop();
  
    progEnd_DB(500);
    
}

/**************************** week 4 Assignment 2********************************/

void week4_2_DB(void){
    
    IR_Start();
    onYourMark_DB();  //move motor to the first line
    
    struct sensors_ dig;
    reflectance_start();
    reflectance_set_threshold(13000, 13000, 11000, 11000, 13000, 13000);
    
    IR_wait();
    reflectance_digital(&dig);
    
    while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(125,0);
    }
    motor_forward(0,0);
    vTaskDelay(300);
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);   
        motor_forward(125,0);
        if(dig.L1 != 1){
            motor_turn(50,0,200);   
        }else if(dig.R1 != 1){
            motor_turn(0,50,204);   
        }else{
            motor_forward(125,0);   
        }
    }
    motor_forward(0,0);
    motor_stop();
    
    progEnd_DB(500);
}

/******************************* week 4 Assignment 3 ****************************************/
void week4_3_DB(void){
    
    IR_Start();
    onYourMark_DB();       //move motor to the first line
    
    struct sensors_ dig;
    reflectance_start();
    reflectance_set_threshold(13000, 13000, 11000, 11000, 13000, 13000);
    
    IR_wait();
    uint32_t line = 0; 
    //reflectance_digital(&dig);
    motor_forward(50,1300);
    
    while(line < 5){
        while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(50,0);
        }
        line++;
        if(line == 2){
            motor_turn(0,75,700);
            while(dig.L2 == 0 || dig.R2 == 0){
                reflectance_digital(&dig);
                if(dig.L1 != 1 && dig.L2 == 0){
                    motor_turn(50,0,100);   
                }else if(dig.R1 != 1 && dig.R2 == 0){
                    motor_turn(0,50,100);   
                }else if(dig.L1 != 0 && dig.R1 != 0){
                    motor_forward(50,0); 
                }   
            }
        }
        //motor_forward(0,0);
        //vTaskDelay(1000);
        if(line == 3 || line == 4){
            motor_turn(75,0,600);
            while(dig.L2 == 0 || dig.R2 ==0){
                reflectance_digital(&dig);
                if(dig.L1 != 1 && dig.L2 == 0 ){
                    motor_turn(50,0,100);   
                }else if(dig.R1 != 1 && dig.R2 == 0 ){
                    motor_turn(0,50,100);   
                }else if(dig.L1 != 0 && dig.R1 != 0){
                    motor_forward(50,0);   
                }
            }
        }
    }
    
    motor_forward(0,0);
    motor_stop();
    
    progEnd_DB(500);
    
}






















































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

void tankTL120_DB(uint32_t delay){
    SetMotors(0,0,25,125,delay);
}

void tankRandTL_DB(uint32_t delay){
    SetMotors(1,0,0,200,delay);   
}

void tankRandTR_DB(uint32_t delay){
    SetMotors(0,1,200,0,delay);
}

int randTurnLR_DB(){
    int TurnLR;
    TickType_t rand = xTaskGetTickCount();
    TurnLR = rand % 2;
    return TurnLR;
}

int randTurnDeg_DB(){
    int deg = 0;
    TickType_t rand = xTaskGetTickCount();
    deg = rand % 1000;
    if(deg < 263){
        deg = 263; 
        return deg;
    }else if (deg > 789){
        deg = 789;
        return deg;
    }else{
        return deg;
    };
}

void onYourMark_DB(){
    struct sensors_ dig;
    reflectance_start();
    reflectance_set_threshold(13000, 13000, 11000, 11000, 13000, 13000); 
    // set center sensor threshold to 11000 and others to 9000
        
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
   
    while(SW1_Read());
    BatteryLed_Write(true);
    vTaskDelay(200);
    BatteryLed_Write(false);
    reflectance_digital(&dig); 
    motor_forward(125,0);
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    motor_forward(0,0);  
}


//void SetMotors(uint8 left_dir, uint8 right_dir, uint8 left_speed, uint8 right_speed, uint32 delay)

/* [] END OF FILE */
