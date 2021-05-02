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
#include "DBproject.h"

#define ZUMO_TOPIC_READY "Zumo06/ready"
#define ZUMO_TOPIC_START "Zumo06/start"
#define ZUMO_TOPIC_STOP "Zumo06/stop"
#define ZUMO_TOPIC_TIME "Zumo06/time"
#define ZUMO_TOPIC_OBSTACLE "Zumo06/obstacle"
#define ZUMO_TOPIC_MISS "Zumo06/miss"
#define ZUMO_TOPIC_POSITION "Zumo06/position"


/********************** SUMO Project ***************************/

void SUMO_DB(void){
    uint32_t startTime;
    uint32_t obstacleTime;
    uint32_t endTime;
    sumo_startUp(1,1,1,1,1);
    startTime = xTaskGetTickCount();
    
    sumo_along_line();
    sumo_onTheMark();
    print_mqtt(ZUMO_TOPIC_READY, "zumo");
    
    IR_wait();
    startTime = xTaskGetTickCount();
    sumo_enterance();
    print_mqtt(ZUMO_TOPIC_START,"%d", startTime);
    while(SW1_Read()){
        struct sensors_ dig;
        reflectance_digital(&dig);
        
        while((dig.L1 != 1 && dig.R1 != 1)){
            reflectance_digital(&dig);
            motor_forward(255,0);
            int d = Ultra_GetDistance();
            if(d < 2){
            motor_forward(0,0);
            obstacleTime = xTaskGetTickCount();
            print_mqtt(ZUMO_TOPIC_OBSTACLE, "%d", obstacleTime);
            motor_backward(255, 100);
            sumo_motorTurn(sumo_randDeg());
            motor_forward(255,0);
            } 
        }
        sumo_motorTurn(120);
        motor_forward(255,0); 
    }
    endTime = xTaskGetTickCount();
    end_sumo(startTime,endTime);
}

/************************************ Line Project ****************************************/

void line_project(void){
    uint32_t startTime;
    uint32_t endTime;
    struct sensors_ dig;
    
    line_startup(1,1,0,1,1);
    line_onTheMark();
    print_mqtt(ZUMO_TOPIC_READY, "line");
    
    IR_wait();
    startTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_START, "%d", startTime);
    
    reflectance_digital(&dig);    
    int i = 0;
    while(i < 2){
        while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(255,0);
        }
        while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
            reflectance_digital(&dig);   
            motor_forward(255,0);
            if(dig.L1 != 1 && dig.R1 == 1){
                line_motorTurn(3);
                reflectance_digital(&dig); 
            }else if(dig.L1 == 1 && dig.R1 != 1){
                 line_motorTurn(-3); 
                 reflectance_digital(&dig);  
            }else if(dig.L1 != 1 && dig.R1 != 1){
                 uint32_t missTime = xTaskGetTickCount();
                 print_mqtt(ZUMO_TOPIC_MISS, "%d", missTime);   
            }    
        }
        i++;
    }    
    end_line();
    endTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_STOP,"%d", endTime);
    print_mqtt(ZUMO_TOPIC_TIME,"%d %02ds",(endTime - startTime),(endTime - startTime)/1000);
}


/************************************* Maze Project ********************************************/
# define safeDist 10
# define leftedge -3
# define rightedge 3
void Maze(void){
    Maze_startup(1,1,1,1,1);
    uint32_t startTime;
    struct sensors_ dig;
    int x, y, obsDist;
    x = 0;
    y = -1;
    
    obsDist = 0;
    Maze_grid_following();
    print_mqtt(ZUMO_TOPIC_READY, "Maze");
        
    IR_wait();
    startTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_START, "%d", startTime);
    
    while(true){
        obsDist = Ultra_GetDistance();
        while(obsDist > safeDist){
            reflectance_digital(&dig);
            if(y == 11 && x > 0){
                MazeTurn(-90);
                while(x > 0){
                    Maze_grid_following();
                    x--;
                    Mqtt_print(x,y);
                }
                MazeTurn(90);
            }else if(y == 11 && x < 0){
                MazeTurn(90);
                while(x < 0){
                Maze_grid_following();
                x++;
                Mqtt_print(x,y);   
                }
                MazeTurn(-90);
            }else if(y == 11 && x == 0){
                while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
                motor_forward(100,0);   
                }
            } 
            Maze_grid_following();
            y++;
            Mqtt_print(x,y);
            if(y == 13){
                Maze_end_line(startTime);
            }
            obsDist = Ultra_GetDistance();
        }
        if(randTurn_Maze() == 1){
            MazeTurn(-90);
            obsDist = Ultra_GetDistance();
            while(obsDist < safeDist){
                MazeTurn(180);
                obsDist = Ultra_GetDistance();
                goto right;
            }
            left:
            Maze_grid_following();
            x--;
            Mqtt_print(x, y);
            MazeTurn(90);
            if(x == leftedge){
                obsDist = Ultra_GetDistance();
                while(obsDist > safeDist && y < 11){
                    Maze_grid_leftedge();
                    y++;
                    Mqtt_print(x,y);
                    obsDist = Ultra_GetDistance();
                }
                while(obsDist < safeDist){
                    MazeTurn(90);
                    Maze_grid_following();
                    MazeTurn(-90);
                    obsDist = Ultra_GetDistance();
                    x++;
                    Mqtt_print(x,y); 
                }
            }
        }else{
            MazeTurn(90);
            obsDist = Ultra_GetDistance();
            while(obsDist < safeDist){
                MazeTurn(-180);
                obsDist = Ultra_GetDistance();
                goto left;
            }
            right:
            Maze_grid_following();
            x++;
            Mqtt_print(x, y);
            MazeTurn(-90);
            if( x == rightedge){
                obsDist = Ultra_GetDistance();
                while(obsDist > safeDist && y < 11){
                    Maze_grid_rightedge();
                    y++;
                    Mqtt_print(x,y);
                    obsDist = Ultra_GetDistance();
                }
                while(obsDist < safeDist){
                    MazeTurn(-90);
                    Maze_grid_following();
                    MazeTurn(90);
                    obsDist = Ultra_GetDistance();
                    x--;
                    Mqtt_print(x,y);
                }           
            }
        }
    }
}
    
    
    
    
    
    
    
    
    
    

    
    
    
    
    
    
    
    
    
    
    
    
    
    















/******************************** SUMO- Custom Functions  ********************************/
void sumo_startUp(int motor,int IR, int ultra, int reflet, int btn){
    if(motor == 1){
        motor_start();
        motor_forward(0,0);
    }
    if(ultra == 1){
        Ultra_Start();
    }
    if(reflet == 1){
        reflectance_start();
        reflectance_set_threshold(11000,12000,14000,14000,12000,11000);
    }
    if(btn == 1){
        while(SW1_Read());
        BatteryLed_Write(true);
	    vTaskDelay(200); 
        BatteryLed_Write(false);
    }
    if(IR == 1){
        IR_Start();
    }
}

void sumo_along_line(void){
    struct sensors_ dig;
    reflectance_digital(&dig); 
    motor_forward(50,0);
    while(!(dig.L1 == 1 && dig.R1== 1)){
        reflectance_digital(&dig);
    }
    motor_forward(50,0);
    while(!(dig.L1 == 0 && dig.R1 == 0)){
        reflectance_digital(&dig);   
    }
    if(dig.L3 == 0){
        motor_forward(50,200);
        sumo_motorTurn(45);
    }else if(dig.R3 == 0){
        motor_forward(50,200);
        sumo_motorTurn(-45);
    }
}

void sumo_onTheMark(void){
    struct sensors_ dig;
    reflectance_digital(&dig); 
    motor_forward(50,0);
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    motor_forward(0,0);  
}

void sumo_enterance(void){
    motor_forward(255,100);
}

int sumo_randDeg(void){
    int deg;
    TickType_t rand = xTaskGetTickCount();
    deg = rand/10 % 180;
    return deg;
}

void sumo_motorTurn(int16_t degree){
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
    uint32 delay = (correction * 1050)/360;
    SetMotors(leftState,rightState, 100, 100, delay);
}

void end_sumo(uint32_t startTime, uint32_t endTime){
    motor_forward(0,0);
    motor_stop();
    print_mqtt(ZUMO_TOPIC_STOP,"%d", endTime);
    print_mqtt(ZUMO_TOPIC_TIME,"%d %02ds",(endTime - startTime),(endTime - startTime)/1000);
    
 }

/**************************************** Line-Custom Functions *********************************/
void line_startup(int motor,int IR, int ultra, int reflect, int btn){
      
    if(motor == 1){
        motor_start();
        motor_forward(0,0);
    }
    if(ultra == 1){
        Ultra_Start();
    }
    if(reflect == 1){
        reflectance_start();
        reflectance_set_threshold(12000,13000,14000,14000,13000,12000);
    }
    if(btn == 1){
        while(SW1_Read());
        BatteryLed_Write(true);
	    vTaskDelay(200); 
        BatteryLed_Write(false);
    }
    if(IR == 1){
        IR_Start();
    }
}

void line_onTheMark(void){
    struct sensors_ dig;
    reflectance_digital(&dig); 
    motor_forward(80,0);
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    motor_forward(0,0);  
}

void line_motorTurn(int16_t degree){
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
    uint32 delay = (correction * 1050)/360;
    SetMotors(leftState,rightState, 100, 100, delay);
}

void end_line(void){
    motor_forward(0,0);
    motor_stop();
 }

/**************************************** Maze Function **************************************/
void Maze_startup(int motor,int IR, int ultra, int reflect, int btn){
      
    if(motor == 1){
        motor_start();
        motor_forward(0,0);
    }
    if(ultra == 1){
        Ultra_Start();
    }
    if(reflect == 1){
        reflectance_start();
        reflectance_set_threshold(14000,14000,14000,14000,14000,14000);
    }
    if(btn == 1){
        while(SW1_Read());
        BatteryLed_Write(true);
	    vTaskDelay(200); 
        BatteryLed_Write(false);
    }
    if(IR == 1){
        IR_Start();
    }
}

int randTurn_Maze(void){
    int TurnLR;
    TickType_t rand = xTaskGetTickCount();
    TurnLR = rand/1000 % 2;
    return TurnLR;
}

void MazeTurn(int16_t degree){
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
    uint32 delay = (correction * 1050)/360;
    SetMotors(leftState,rightState, 100, 100, delay);
    motor_forward(0,0);
    vTaskDelay(300);
}

void Maze_grid_following(void){
    struct sensors_ dig;
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
        motor_forward(100,0);
    }
    while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(100,0);
    }
    motor_forward(100,120);
    motor_forward(0,0);
}

void Maze_grid_leftedge(void){
    struct sensors_ dig; 
    while(!(dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
        motor_forward(100,0);
    }
    while(!(dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(100,0);
    }
    motor_forward(100,120);
    motor_forward(0,0); 
}

void Maze_grid_rightedge(void){
    struct sensors_ dig; 
    while(!(dig.L3 == 1 && dig.L2 == 1)){
        reflectance_digital(&dig);
        motor_forward(100,0);
    }
    while(!(dig.L3 == 0 && dig.L2 == 0)){
        reflectance_digital(&dig);
        motor_forward(100,0);
    }
    motor_forward(100,120);
    motor_forward(0,0); 
}

void Mqtt_print(int x, int y){
    print_mqtt(ZUMO_TOPIC_POSITION, "%d %d", x, y);   
}

void Maze_end_line(uint32_t startTime){
    motor_forward(0,0);
    motor_stop();
    uint32_t endTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_STOP, "%d", endTime);
    print_mqtt(ZUMO_TOPIC_TIME, "%d,%02ds", (endTime - startTime), (endTime - startTime)/1000);
    bool led = false;
    while(true){
        BatteryLed_Write(led^=1);
        vTaskDelay(300);
    }
 }


/* [] END OF FILE */
