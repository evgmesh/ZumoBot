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
