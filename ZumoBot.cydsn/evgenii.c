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

#define PRESSED 0
#define RELEASED 1
#define BUTTON_TOPIC "Group6_Evg/button"
#define TURN_TOPIC "Group6_Evg/turn"


/*******************************************************weekly assignments***************************************/

void week5_2_evg(void) 
{
    startUp(1,1,0,0,1);
    while(true)
    {
      motor_forward(200, 50);
      int d = Ultra_GetDistance();
        //Print the detected distance (centimeters)
      //printf("distance = %d\r\n", d);
        vTaskDelay(200);
        if(d < 15){
            int random = randomEvg(0, 1);
            motor_forward(0,0);         
            motor_backward(100, 150);
            
            if (random == 1) {
               motor_turn(255,0,206);
               print_mqtt(TURN_TOPIC, "/ Turn direction is right, %i", random);
            } else {
               motor_turn(0,255,206);
               print_mqtt(TURN_TOPIC, "/ Turn direction is left, %i", random);
            } 
        }
        if(SW1_Read() == PRESSED){
            end(); 
            break;
        }
    }
}



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
    
    startUp(1,0,1,1,0);
    
    reflectance_digital(&dig);
    while(count<5) 
    {
        driveForward();
        count++;
      if(count == 1)
        {
         motor_forward(0,0);
         startUp(0,1,0,0,0);
        }
    }
    printf("Number of lines is %i\n", count);
    end();
}


//week 4 Exercise 2

void week4_2_evg(void) 
{
    struct sensors_ dig;
    startUp(1,1,1, 1, 0);
    
    reflectance_digital(&dig); 
    driveForward();
    
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
                tankTurnEvg(150,0,0);
                reflectance_digital(&dig); 
            }else if(dig.L2 == 1)
             {   
                tankTurnEvg(0,200,0);
                reflectance_digital(&dig); 
             }
        }
    end();
}
//week 4 Exercise 3

void week4_3_evg(void) 
{
    uint32_t count = 0;
    struct sensors_ dig;
    
    startUp(1, 0, 1, 1, 0);

    reflectance_digital(&dig);
    while(count<5) 
    {
        driveForward();
        count++;
        printf("one loop, %i lines passed by\n", count);
        reflectance_digital(&dig);
        // wait on first line
        if(count == 1)
        {
            motor_forward(0,0);
            startUp(0,1,0,0, 0);
        } else if(count == 2) 
          {
              //turn left on second line  
              while(!(dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0)  ) 
              {
                    motor_turn(4,250,50);
                    reflectance_digital(&dig);
              }
        } else if(count == 3) 
          {
            //turn right on third line
            while(!(dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0) ) 
            {
                motor_turn(230,10,70);
                reflectance_digital(&dig);
            }
        } else if(count == 4) 
          {
            //turn right on fourth line
            while(!(dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0)) 
            {
                motor_turn(230,10,50);
                reflectance_digital(&dig);
            }
          } 
    }
    end();
}

//week 5 Exercise 1

void week5_1_evg(void) 
{
    uint32_t firstBtnPress = 0, lastBtnPress = 0, diff = 0;
    
    printf("\n\n clearing the buffer\n\n");
    printf("Press a button\n");
    
    
    while(true)
    {
        while(SW1_Read() == RELEASED) vTaskDelay(1); // waiting in the loop until user presses button
        firstBtnPress = xTaskGetTickCount();
        diff = firstBtnPress-lastBtnPress;
        
        while(SW1_Read() == PRESSED) vTaskDelay(1); // waiting in the loop while user pressing the button
        print_mqtt(BUTTON_TOPIC, "/ Milliseconds since boot or the last button press is %d, for human: %02im:%02i.%03i", \
                    diff, diff/1000/60%60, diff/1000%60, diff%1000);
        lastBtnPress = firstBtnPress;
    }
}

//week 5 Exercise 2


//week 5 Exercise 3
void week5_3_evg(void) 
{
    
}

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



void tankTurnEvg(uint8 l_speed, uint8 r_speed, uint32 delay){
    SetMotors(0,0, l_speed, r_speed, delay);
}

void end(void) {
    motor_forward(0,0);         
    motor_stop();
}

// function to start assets. 1 as parameter starts motor, infrared, reflectance, button in same order 
void startUp(int motor, int IR, int reflectance, int button, int ultra) {
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
    if(ultra == 1) {
        Ultra_Start();
    }
    
    // more to add here
}


/* [] END OF FILE */
