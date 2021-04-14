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


/************************ week4 Ex.1 ************************************/
void group6_Ex1(void) 
{
    uint32_t count = 0;
    struct sensors_ dig;
    
    startUp(1,0,1,1,0);
    
    reflectance_digital(&dig);
    while(count<5) 
    {
        driveForward(100,0);
        count++;
      if(count == 1)
        {
         motor_forward(0,0);
         startUp(0,1,0,0,0);
        }
    }
    //printf("Number of lines is %i\n", count);
    end();
}


/************************ week4 Ex.2 ************************************/
void group6_Ex2(void) 
{
    struct sensors_ dig;
    startUp(1,1,1,1,0);
    
    reflectance_digital(&dig); 
    driveForward(100,0);
    
        reflectance_digital(&dig); 
        while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0))
        {
             reflectance_digital(&dig);
             motor_forward(125,0);
        }
        reflectance_digital(&dig); 
        while(dig.L3 == 0 && dig.R3 == 0)
        {
            motor_forward(50,0);
            reflectance_digital(&dig); 
            
            if(dig.R2 == 1)
            {   
                motor_turn(150,0,0);
                reflectance_digital(&dig); 
            }else if(dig.L2 == 1)
             {   
                motor_turn(0,200,0);
                reflectance_digital(&dig); 
             }
        }
    end();
}


/************************ week4 Ex.3 ************************************/

void group6_Ex3(void){
    
    onYourMark_DB();       //move motor to the first line
    
    IR_wait();
    int line = 0; 
    
    motor_forward(50,0);
    
    while(line < 5){
        lineDetector_DB(); //detect the black line 
                
        line++;             //count the number of black lines
        
        if(line == 2){
            motor_turn(0,75,700);
            followTheLine_DB();  //control the movement by the line
        }
        
        if(line == 3 || line == 4){
            motor_turn(75,0,601);
            followTheLine_DB();
        }
    }
    flameout_DB();
    
}










































/***************************** Group6 Custom Functions **************************

void driveForward(void){
    struct sensors_ dig;
    reflectance_start(); 
    reflectance_digital(&dig);
    //drives forward when sensors 2 and 3 are on black
    while(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)
    {
        motor_forward(100,0);
        reflectance_digital(&dig); 
    }
    //drives forward when sensors 2 and 3 are on white
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



void tankTurnEvg(uint8_t l_speed, uint8_t r_speed, uint32_t delay){
    SetMotors(0,0, l_speed, r_speed, delay);
}

void end(void) {
    motor_forward(0,0);         
    motor_stop();
}

// function to start assets. 1 as parameter starts motor, infrared, reflectance, button in same order 
void startUp(int motor, int IR, int reflectance, int button) {
    if(motor == 1){
        motor_start();  
        motor_forward(0,0);
    }
    if(IR == 1){
        IR_Start();
        IR_wait();    
    }
    if(reflectance == 1){
        reflectance_start(); 
        reflectance_set_threshold(11000, 11000, 14000, 14000, 11000, 11000);
    }
    if(button == 1) {
        while(SW1_Read());
    }

}

void onYourMark_DB(void){
    IR_Start();
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    struct sensors_ dig;
    reflectance_start();
    reflectance_set_threshold(13000, 13000, 11000, 11000, 13000, 13000); 
    // set center sensor threshold to 11000 and others to 9000
        
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

void followTheLine_DB(void){
    struct sensors_ dig;
    reflectance_digital(&dig);
    while(!(dig.L2 == 1 && dig.R2 == 1)){
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

void lineDetector_DB(void){
    struct sensors_ dig;
    reflectance_digital(&dig);
    if(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
            while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
                reflectance_digital(&dig);
                motor_forward(50,0);
                }
        }else if((!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1))){
            while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
                reflectance_digital(&dig);
                motor_forward(50,0);
                }
        }   
    
}
void flameout_DB(void){
    motor_forward(0,0);
    motor_stop();
    
    progEnd_DB(500); 
}


************************************************************************/









