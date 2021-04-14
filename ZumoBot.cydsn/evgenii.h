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

#ifndef EVGENII_H_
#define EVGENII_H_

#include <stdint.h>
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

void progEndEvg(uint32_t delay);
void tankTLEvg(uint8_t speed, uint32_t delay);
void tankTREvg(uint8_t speed, uint32_t delay);
void tankTurnEvg(int16 degree);
int randomEvg(int min, int max);
void startUp(int motor, int IR, int reflectance, int button, int ultra);
void driveForward();
void end(void);
void week3_1_evg(void);
void week3_2_evg(void);
void week3_3_evg(void);
void week4_1_evg(void);
void week4_2_evg(void);
void week4_3_evg(void);
void week5_1_evg(void);
void week5_2_evg(void);
void week5_3_evg(void);

#endif

/* [] END OF FILE */
