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


/************************ week5 Ex.1 ************************************/
#define PRESSED 0
#define RELEASED 1
#define BUTTON_TOPIC "Group6/button"
#define TURN_TOPIC "Group6/turn"
#define LAP_TOPIC "Group6/lap"

void group6_Ex1(void){
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
        print_mqtt(BUTTON_TOPIC, " Time: %d ms\n", time );
        btn1Time = btn2Time;
        
    }
    motor_stop();

}

/************************ week5 Ex.2 ************************************/
void group6_Ex2(void) 
{
    startUp(1,1,0,0,1);
    while(SW1_Read() == RELEASED)
    {
      motor_forward(150, 80);
        if(Ultra_GetDistance() < 10){
            motor_forward(0,0);         
            motor_backward(150, 150);
            
            if (rand()%2 == 1) {
               tankTurnEvg(90);
               motor_forward(0,0);
               print_mqtt(TURN_TOPIC, "/ Turn direction - right");
            } else {
               tankTurnEvg(-90);
               motor_forward(0,0);
               print_mqtt(TURN_TOPIC, "/ Turn direction - left");
            } 
            
        }
    }
    end();
}


/************************ week5 Ex.3 ************************************/

void group6_Ex3(void){
    onYourMark2_DB();    //move the motor to first line
    struct sensors_ dig;
    
    uint32_t startTime = 0;
    uint32_t endTime = 0;
    printf("\n\n\n!!!BOOT BOOT!!!\n\n");
    
    IR_wait();
    for(int i = 0; i < 4; i++){
        startTime = xTaskGetTickCount();
        
        while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
            reflectance_digital(&dig);
            motor_forward(50,0);
        }              
        motor_forward(50,0);
        while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
            reflectance_digital(&dig);
        }
        endTime = xTaskGetTickCount();
        motor_forward(0,0);
        vTaskDelay(300);
        print_mqtt(LAP_TOPIC, "startTime: %d, endTime: %d, Elapsed time: %dms", startTime, endTime, (endTime - startTime));
    }
    flameout_DB();
}










































/***************************** Group6 Custom Functions **************************

void driveForward(uint8 speed, uint32 delay){
    struct sensors_ dig;
    reflectance_start(); 
    reflectance_digital(&dig);
    //drives forward when sensors 2 and 3 are on black
    while(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)
    {
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
    }
    //drives forward when sensors 2 and 3 are on white
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1))
    {
        motor_forward(speed,delay);
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

void tankTurnEvg(int16 degree){
    
    uint8 leftState, rightState, correction;
    if(degree>=0) {
        leftState=0;
    } else{
        leftState=1;
    }
    if(degree<0) {
        rightState=0;
        correction= (degree * -1) %360;
    } else{
        rightState=1;
        correction= degree % 360;
    }
    uint32 delay = (correction * 1048)/360;
    
    SetMotors(leftState,rightState, 100, 100, delay);
}

void end(void) {
    motor_forward(0,0);         
    motor_stop();
}

// function to start assets. 1 as parameter starts motor, infrared, reflectance, button in same order 
void startUp(int motor, int IR, int reflectance, int button, int ultra) {
    printf("\n\n clearing the buffer\nBoot!\n");
    if(motor == 1){
        motor_start();  
        motor_forward(0,0);
    }
    if(IR == 1){
        IR_Start();
        printf("to start press IR send\n");
        IR_flush();
        IR_wait();    
    }
    if(reflectance == 1){
        reflectance_start(); 
        reflectance_set_threshold(11000, 11000, 14000, 14000, 11000, 11000);
    }
    if(button == 1) {
        while(SW1_Read());
    }
    if(ultra == 1) {
        Ultra_Start();
    }
    
    // more to add here
}

void flameout_DB(void){
    motor_forward(0,0);
    motor_stop();
    printf("\n\n Press the reset and Try it again!!\n\n");
    
    progEnd_DB(500); 
}

void onYourMark2_DB(void){
    IR_Start();
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    struct sensors_ dig;
    reflectance_start();
    reflectance_set_threshold(15000, 15000, 16000, 16000, 15000, 15000); 
    // set center sensor threshold to 11000 and others to 9000
        
    reflectance_digital(&dig); 
    motor_forward(125,0);
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    
    while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(50,0);
    }
    
    motor_forward(0,0); 
    
}
************************************************************************/









