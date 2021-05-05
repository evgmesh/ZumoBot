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
#include <math.h>
    
    
//***************choose project******************//

#define MAZE 0   
#define LINE 0
#define SUMO 1
    
#define PRESSED 0
#define RELEASED 1
#define BUTTON_TOPIC "Zumo06/button/"
#define TURN_TOPIC "Zumo06/turn/"
#define LAP_TOPIC "Zumo06/lap/"
#define READY_TOPIC "Zumo06/ready/"
#define START_TOPIC "Zumo06/start/"
#define STOP_TOPIC "Zumo06/stop/"
#define TIME_TOPIC "Zumo06/time/"
#define MISS_TOPIC "Zumo06/miss/"
#define LINE_TOPIC "Zumo06/line/"
#define OBST_TOPIC "Zumo06/obstacle/"
#define POSI_TOPIC "Zumo06/position/"
#define SPEED 120
#define FAST 255
#define LEFT -1
#define RIGHT 1
#define FORWARD 0
#define MOTOR 1
#define INFRA 1
#define REFLECT 1
#define BUTTON 1
#define ULTRA 1

void progEndEvg(uint32_t delay);
void turnLeft(uint8_t speed, uint32_t delay);
void turnRight(uint8_t speed, uint32_t delay);
void tankTurnEvg(int16 degree);
int randomEvg(int min, int max);
void startUp(int motor, int ir, int reflectance, int button, int ultra);
void driveForward(uint8 speed, uint32 delay);
void end(void);
void end_mqtt(uint32_t start, uint32_t stop);
uint16_t sense(struct sensors_ dig);
int noRestrict(int *Y);
bool obst(void);
void leftInMaze(void);
void rightInMaze(void);

void driveThruMaze(uint8 speed, uint32 delay);


void week3_1_evg(void);
void week3_2_evg(void);
void week3_3_evg(void);
void week4_1_evg(void);
void week4_2_evg(void);
void week4_3_evg(void);
void week5_1_evg(void);
void week5_2_evg(void);
void week5_3_evg(void);

void sumo_wrestling(void);
void line_follower(void);
void maze(void);


#endif

/* [] END OF FILE */
