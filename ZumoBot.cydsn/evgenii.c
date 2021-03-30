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


//week 3 Ex.1

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





void progEnd(uint32_t delay) {
    bool led = false;
    while(true){
     BatteryLed_Write(led^=1);
     vTaskDelay(delay);   
    }
}
void tankTL(uint8_t speed, uint32_t delay) {
    SetMotors(1, 0, speed, speed, delay);
}

void tankTR(uint8_t speed, uint32_t delay) {
    SetMotors(0, 1, speed, speed, delay);
}

void tankTurn(uint8 l_speed, uint8 r_speed, uint32 delay){
    SetMotors(0,0, l_speed, r_speed, delay);
}
/* [] END OF FILE */
