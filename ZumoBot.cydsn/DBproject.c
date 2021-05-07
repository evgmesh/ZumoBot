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

void SUMO_Project(void){
    uint32_t startTime;
    uint32_t obstacleTime;
    uint32_t endTime;
    sumo_startUp(1,1,1,1,1);                                //Startup the robot with all sensors and ultrasonic and button etc.
    startTime = xTaskGetTickCount();
    
    sumo_along_line();                                      //following the line for entry
    sumo_onTheMark();                                       //stop and wait for IR button press
    print_mqtt(ZUMO_TOPIC_READY, "sumo");
    
    IR_wait();
    startTime = xTaskGetTickCount();
    sumo_enterance();                                       //enter the SUMO ring
    print_mqtt(ZUMO_TOPIC_START,"%d", startTime);
    while(SW1_Read()){
        struct sensors_ dig;
        reflectance_digital(&dig);
        
        while((dig.L1 != 1 && dig.R1 != 1)){                //Loop around inside ring till it detects edge
            reflectance_digital(&dig);
            motor_forward(255,0);
            int d = Ultra_GetDistance();
            if(d < 2){
            motor_forward(0,0);
            obstacleTime = xTaskGetTickCount();
            print_mqtt(ZUMO_TOPIC_OBSTACLE, "%d", obstacleTime);
            motor_backward(255, 100);
            sumo_motorTurn(sumo_randDeg());                 //when detect obstacles, start random turn
            motor_forward(255,0);
            } 
        }
        sumo_motorTurn(120);                                //On the ring edge, it turns 120 degree only
        motor_forward(255,0); 
    }
    endTime = xTaskGetTickCount();
    end_sumo(startTime,endTime);
}

/************************************ Line Project ****************************************/

void LINE_Project(void){
    uint32_t startTime;
    uint32_t endTime;
    struct sensors_ dig;
    
    line_startup(1,1,0,1,1);                               //Startup all neccessary detectors or buttons
    line_onTheMark();                                      //move till the first line and waiting for order!
    print_mqtt(ZUMO_TOPIC_READY, "line");
    
    IR_wait();
    startTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_START, "%d", startTime);
    
    reflectance_digital(&dig);    
    int i = 0;
    while(i < 2){                                          //running twice when detects lines
        while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(255,0);
        }
        while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
            reflectance_digital(&dig);   
            motor_forward(255,0);
            if(dig.L1 != 1 && dig.R1 == 1){             //center sensors used to detect if miss, if so, turn left
                line_motorTurn(3);                      //turning degree +3 degree to left
                reflectance_digital(&dig); 
            }else if(dig.L1 == 1 && dig.R1 != 1){       //if miss, turn right
                 line_motorTurn(-3);                    //turning degree means -3 degree to right
                 reflectance_digital(&dig);  
            }else if(dig.L1 != 1 && dig.R1 != 1){       //if both sensors miss, then print miss message
                 uint32_t missTime = xTaskGetTickCount();
                 print_mqtt(ZUMO_TOPIC_MISS, "%d", missTime);   
            }    
        }
        i++;                                            //loop increase
    }    
    end_line();
    endTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_STOP,"%d", endTime);
    print_mqtt(ZUMO_TOPIC_TIME,"%d %02ds",(endTime - startTime),(endTime - startTime)/1000);
}


/************************************* Maze Project ********************************************/
#define left -90
#define right 90
#define safeDist 10
#define leftedge -3
#define rightedge 3

void MazeIII(void){
    uint32_t startTime;
    int x, y, obsDist;
    x = 0;
    y = -1;
    obsDist = 0;
    bool goleft = false;
    bool goright = false;
    MazeIII_startup(1,1,1,1,1);
    MazeIII_onTheMark();
    
    //to be ready to start on the line
    IR_wait();
    startTime = xTaskGetTickCount();
    print_mqtt(ZUMO_TOPIC_START, "%d", startTime);
    
   //between row 0 and row 10, robot movement
    while(y < 11){
        obsDist = Ultra_GetDistance();
        while(obsDist > safeDist && y < 11){
            obsDist = Ultra_GetDistance();
            y++;
            MazeIII_Mqtt_print(x,y);
            MazeIII_grid_following();
            obsDist = Ultra_GetDistance();
        }
        
        //random determine direction of left or right
        if(obsDist < safeDist){          
            int j = MazeIII_random();
            switch(j){
                case(0):{
                    goleft = true;
                    break;  
                }
                case(1):{
                    goright = true;
                    break;
                }
            }
        } 
        
        //move on the left side of y axis 
        while(goleft && x > leftedge && y < 11){
            obsDist = Ultra_GetDistance();
            while(obsDist > safeDist && y < 11){
                y++;
                MazeIII_Mqtt_print(x, y);
                MazeIII_grid_following();
                obsDist = Ultra_GetDistance();
            }
            if(y < 11){
                MazeIII_Turn(left);
            }
            obsDist = Ultra_GetDistance();
            if(obsDist > safeDist && y < 11){ //check if there is obstacel
                x--;
                MazeIII_Mqtt_print(x, y);
                MazeIII_grid_following();
                MazeIII_Turn(right);  
            }else if(obsDist < safeDist){ //obstacle exists, then no goal on left side
                MazeIII_Turn(right);
                goleft = false; 
            }
        }
        
        //when robot moves on left edge
        if(goleft && x == leftedge){ 
            obsDist = Ultra_GetDistance();
            while(obsDist > safeDist && y < 11){
                y++;
                MazeIII_Mqtt_print(x, y);
                MazeIII_leftedge();
                obsDist = Ultra_GetDistance();
            }
            MazeIII_Turn(right); 
            obsDist = Ultra_GetDistance();
            
            while(obsDist > safeDist && x < 0){ //when reach the left edge, no need 
                                                //to check obstacle but go straight to poisiiton x0
                x++;                            
                MazeIII_Mqtt_print(x,y);
                MazeIII_grid_following();
                obsDist = Ultra_GetDistance();
            }
            MazeIII_Turn(left);
            goleft = false;                     //left side switch off
        }  
                
        // Robot moves to the right side
        while(goright && x < rightedge && y < 11){
            obsDist = Ultra_GetDistance();
            while(obsDist > safeDist && y < 11){
                y++;
                MazeIII_Mqtt_print(x, y);
                MazeIII_grid_following(); 
                obsDist = Ultra_GetDistance();
            }
            if(y < 11){
                MazeIII_Turn(right);
            }
            obsDist = Ultra_GetDistance();
            if(obsDist > safeDist && y < 11){  // check if there is obstacle
                x++;
                MazeIII_Mqtt_print(x, y);
                MazeIII_grid_following();
                MazeIII_Turn(left);  
            }else if(obsDist < safeDist){      //obstacle exists, then no goal on right side
                MazeIII_Turn(right);
                goright = false;               //right side switch off
            }
        }
        //when robot reach the right edge
        if(goright && x == rightedge){
            obsDist = Ultra_GetDistance();
            while(obsDist > safeDist && y < 11){  //move forward if no obstacles
                y++;
                MazeIII_Mqtt_print(x, y);
                MazeIII_rightedge();
                obsDist = Ultra_GetDistance();
            }
            MazeIII_Turn(left); 
            obsDist = Ultra_GetDistance();
            while(obsDist > safeDist && x > 0){    //when reach the right edge, no need 
                                                   //to check obstacle but go straight to poisiiton x0
                x--;
                MazeIII_Mqtt_print(x,y);
                MazeIII_grid_following();
                obsDist = Ultra_GetDistance();
            }
            MazeIII_Turn(right);
            goright = false;
        }  
    }
    
    //after row 11 to the finish line
    while(y <13){
        if(y == 11 && x > 0){               //on right side 
            MazeIII_Turn(left);
            while(x > 0){
                MazeIII_grid_following();
                x--;
                MazeIII_Mqtt_print(x,y);
            }
            MazeIII_Turn(right);
        }else if(y == 11 && x < 0){         //on the left side
            MazeIII_Turn(right);
            while(x < 0){
                MazeIII_grid_following();
                x++;
                MazeIII_Mqtt_print(x,y);   
            }
            MazeIII_Turn(left);
        }else if(y == 11 && x == 0){        //on the position x0
            while(y < 13){             
                MazeIII_grid_following(); 
                y++;
                MazeIII_Mqtt_print(x,y);
            }
        }   
    }  
    MazeIII_end_line(startTime);
}

    
    
 



    
    
    
    
    

    
    
    
    
    
    
    
    
    
    
    
    
    
    















/******************************** SUMO- Custom Functions  ********************************/
void sumo_startUp(int motor,int IR, int ultra, int reflet, int btn){   // startup function to activate all if needed
    printf("\n\n\n!!!BOOT,BOOT!!!\n\n\n");
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

void sumo_along_line(void){                     //automatically turn 45 degree on left or right
                                                //by detecting the line
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
    deg = rand/10 % 180;                        //random degree till 180 degree when encounter obstacle, it could be in 360 range as well
    return deg;
}

void sumo_motorTurn(int16_t degree){                //motor automatic turning function to give tankturn by fill in parameter degree
    uint8 leftState, rightState, correction;
    if(degree>=0) {
        leftState=0;                                //positive degree means right turn
    } else{
        leftState=1;
    }
    if(degree<0) {                                  //negative degree means left turn
        rightState=0;
        correction= (degree * -1) %360;
    } else{
        rightState=1;
        correction= degree % 360;
    }
    uint32 delay = (correction * 1050)/360;         //check with 100 turnning around it about 1050 delay then calculate delay
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
    printf("\n\n\n!!!BOOT,BOOT!!!\n\n\n");  
    
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
void MazeIII_startup(int motor,int IR, int ultra, int reflect, int btn){
    printf("\n\n\n!!!BOOT,BOOT!!!\n\n\n");
    
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

void MazeIII_onTheMark(void){
    struct sensors_ dig;
    reflectance_digital(&dig); 
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        motor_forward(100,0);
        reflectance_digital(&dig);
    }
    motor_forward(100,80);
    motor_forward(0,0); 
    print_mqtt(ZUMO_TOPIC_READY, "Maze");
}

void MazeIII_grid_following(void){                                      //following th grid line then move a bit further turn 90 degree
    struct sensors_ dig;
    while(!(dig.L3 == 1 && dig.L2 == 1 && dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
        motor_forward(255,0);
    }
    while(!(dig.L3 == 0 && dig.L2 == 0 && dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(255,0);
    }
    motor_forward(100,120);                                            //make sure it turn 90 both central sensors are on the line 
    motor_forward(0,0);
}

void MazeIII_Mqtt_print(int x, int y){
    print_mqtt(ZUMO_TOPIC_POSITION, "%d %d", x, y);   
}

int MazeIII_random(void){                           //random turn function to give random value
    int i;
    TickType_t rand = xTaskGetTickCount();
    i = rand/1000 % 2;
    return i;
}

void MazeIII_Turn(int16_t degree){
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
    vTaskDelay(50);
}

void MazeIII_leftedge(void){                        //on the edge, use either side of sensor to detect the lines
    struct sensors_ dig; 
    while(!(dig.R2 == 1 && dig.R3 == 1)){
        reflectance_digital(&dig);
        motor_forward(255,0);
    }
    while(!(dig.R2 == 0 && dig.R3 == 0)){
        reflectance_digital(&dig);
        motor_forward(255,0);
    }
    motor_forward(100,120);
    motor_forward(0,0); 
}

void MazeIII_rightedge(void){                       //on the edge, use either side of sensor to detect the lines
    struct sensors_ dig; 
    while(!(dig.L2 == 1 && dig.L3 == 1)){
        reflectance_digital(&dig);
        motor_forward(255,0);
    }
    while(!(dig.L2 == 0 && dig.L3 == 0)){
        reflectance_digital(&dig);
        motor_forward(255,0);
    }
    motor_forward(100,120);
    motor_forward(0,0); 
}

void MazeIII_end_line(uint32_t startTime){      //end when reach the top, print time elapsed
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
