
/* ========================================Evgenii Meshcheriakov==========================================*/
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
#include <math.h>

/*******************************************************weekly assignments***************************************/


//week 3 Exercise 1

void week3_1_evg(void)
{
    
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    IR_Start();
    IR_wait();
    motor_forward(125,2750);     // moving forward from starting point
    
    turnRight(255, 103);           // 1st turning right 
    motor_forward(125,2350);    // forward on horisontal line
    turnRight(255, 103);           // 2nd turn
    motor_forward(125,2550);
    turnRight(255, 103);           // 2nd turn
  //  tankTurnEvg(164,135,1780);
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
               turnLeft(255, turn);
               printf("\nTurn value is %i so I turn left\n", turn);
            } else {
               turnRight(255, turn);
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
    int count = 0;
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
    printf("Number of lines is %i\n", count);
    end();
}


//week 4 Exercise 2

void week4_2_evg(void) 
{
    struct sensors_ dig;
    startUp(1,1,1, 1, 0);
    
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
//week 4 Exercise 3

void week4_3_evg(void) 
{
    uint32_t count = 0;
    struct sensors_ dig;
    
    startUp(1, 0, 1, 1, 0);

    reflectance_digital(&dig);
    while(count<5) 
    {
        driveForward(100,0);
        count++;
        printf("one loop, %lu lines passed by\n", count);
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
        lastBtnPress = xTaskGetTickCount();
        while(SW1_Read() == RELEASED) vTaskDelay(1); // waiting in the loop until user presses button
        firstBtnPress = xTaskGetTickCount();
        diff = firstBtnPress-lastBtnPress;
        
        while(SW1_Read() == PRESSED) vTaskDelay(1); // waiting in the loop while user pressing the button
        print_mqtt(BUTTON_TOPIC, "/ Milliseconds since boot or the last button press is %d, or: %02dm:%02d.%03ds", \
                    diff, diff/1000/60%60, diff/1000%60, diff%1000);
        lastBtnPress = firstBtnPress;
    }
}

//week 5 Exercise 2

void week5_2_evg(void) 
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


//week 5 Exercise 3

void week5_3_evg(void) 
{
    int lines=0;
    uint32_t irPressed = 0, lineReached = 0, elapsed = 0;
    
    startUp(1,1,1,0,0);
    
    driveForward(100,0);
    IR_wait();
    irPressed = xTaskGetTickCount();
    
    
    while(lines<1){
    driveForward(150,0);
    lines++;
    }
    
    lineReached = xTaskGetTickCount();
    elapsed = lineReached-irPressed;
    print_mqtt(LAP_TOPIC, "time elapsed: %ims", elapsed);
    
    end();
}


//********************************Project***************************************//
                                // Sumo

void sumo_wrestling(void) 
{
    struct sensors_ dig;
    int32_t startTime = 0, stopTime = 0, obst = 0;
    
    startUp(1,1,1,0,1);
    
    IR_wait();
    reflectance_digital(&dig); 
    driveForward(255,0);
    print_mqtt(READY_TOPIC, "zumo");
    IR_flush();
    IR_wait();
    motor_forward(100,100);
    startTime = xTaskGetTickCount();
    print_mqtt(START_TOPIC, " %lu", startTime);
    while(SW1_Read() == RELEASED)
    {
        int angle = randomEvg(45, 180);
        if (Ultra_GetDistance()<3)
        {
            obst = xTaskGetTickCount();
            print_mqtt(OBST_TOPIC, " %lu", obst);
            motor_forward(0,0);
            tankTurnEvg(angle%2 == 1 ? -angle : angle);
        } 
        reflectance_digital(&dig);
        while (sense(dig) > 0)
        {   
            motor_forward(0,0);
            tankTurnEvg(angle%2 == 1 ? -angle : angle);
            reflectance_digital(&dig); 
        }
        motor_forward(250,0);
    }
    stopTime = xTaskGetTickCount();
    end_mqtt(startTime, stopTime);
}

// set value of variable bonus in driveForward to 1 
void line_follower(void) {
   int lines=0;
   struct sensors_ dig;
   uint32_t startTime = 0, stopTime = 0;
    
   startUp(1,1,1,0,0);
    
   IR_wait();  
    while(lines<3)
    {
        reflectance_digital(&dig);
        driveForward(100,0);
        lines++;
        if (lines == 1)
        {
            motor_forward(0,0);
            print_mqtt(READY_TOPIC, "line");
            IR_flush();
            IR_wait(); 
            startTime = xTaskGetTickCount();
            print_mqtt(START_TOPIC, " %i", startTime);
        }
        
    }
    stopTime = xTaskGetTickCount();
    end_mqtt(startTime, stopTime);
}

void maze(void) {
    struct sensors_ dig;
    startUp(1,1,1,0,1);
    int maxX = 3;
    int maxY = 14;
    int Y = -1;
    int X = 0;
    int left = -90;
    int right = 90;
    bool dirUp = true, dirLeft=false, dirRight=false;
    
    IR_wait();              // temp - delete and turn on button in startUp
    driveForward(200,0);
    IR_wait();  
    
    
    
    reflectance_digital(&dig);
    while(Y < 4)
    {
        while(dirUp && Ultra_GetDistance() > 13)
        {
            reflectance_digital(&dig);
            driveThruMaze(SPEED,0);
            Y++;
            print_mqtt(POSI_TOPIC, "case 1 %i %i", X, Y);
            vTaskDelay(100);
        }
        if(dirUp && Ultra_GetDistance() < 13)
        {
            print_mqtt(POSI_TOPIC, "case 2 %i %i", X, Y);
            dirLeft = true;
            dirUp = false;
            motor_forward(200,110);
            tankTurnEvg(left);
            motor_forward(0,0);
            vTaskDelay(100);
        } 
        if(dirLeft && Ultra_GetDistance() > 13)
        {   
            dirLeft = false;
            dirUp = true;
            reflectance_digital(&dig);
            driveThruMaze(SPEED,0);
            X--;
            print_mqtt(POSI_TOPIC, "case 3 %i %i", X, Y);
            
            motor_forward(200,110);
            tankTurnEvg(right);
            motor_forward(0,0);
            vTaskDelay(100);
            if(Ultra_GetDistance() < 13)
            {
                print_mqtt(POSI_TOPIC, "case 4 %i %i", X, Y);
                dirLeft = true;
                dirUp = false;
                
                tankTurnEvg(left);
                motor_forward(0,0);
                vTaskDelay(100);
            }
        }
        if(dirLeft && Ultra_GetDistance() < 13)
        {
            
            dirLeft = false;
            dirRight = true; 
            tankTurnEvg(180);
            motor_forward(0,0);
            print_mqtt(POSI_TOPIC, "case 5 %i %i", X, Y);
            vTaskDelay(100);
        }
        if(dirRight && Ultra_GetDistance() > 13)
        {
            dirRight = false;
            dirUp = true;
            driveThruMaze(SPEED,0);
            X++;
            print_mqtt(POSI_TOPIC, "case 6 %i %i", X, Y);
            motor_forward(200,110);
            tankTurnEvg(left);
            motor_forward(0,0);
            vTaskDelay(100);
            if(Ultra_GetDistance() < 13)
            {
                print_mqtt(POSI_TOPIC, "case 7 %i %i", X, Y);
                dirRight = true;
                dirUp = false;
                
                tankTurnEvg(right);
                motor_forward(0,0);
                vTaskDelay(100);
            }
            
        }
        
    } 
  /*  if(Y==3)
    {
        print_mqtt(STOP_TOPIC, "END");
        end();
    }
        */
    end();
}




void driveForward(uint8 speed, uint32 delay){
    
    // declarations
    struct sensors_ dig;
    reflectance_digital(&dig);
    bool miss = true, bonus = 0;
    
    //drives forward when all sensors are on the line
     while(sense(dig)==63)
    {
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
      //  printf("sense is %i\n", sense(dig));
    }
    //condition when only central sensors are on the line
    //while(sense(dig)!=63 && sense(dig)!=60 && sense(dig)!= 15)
    while(sense(dig)!=63 && sense(dig)!=60 && sense(dig)!= 15)
    {
      //  printf("sense is %i\n", sense(dig));
        //if loosing line from left side of the bot - turn right
        if(dig.L1 == 0 && dig.R1 == 1)
        {
            turnRight(speed,delay);
            reflectance_digital(&dig); 
        } 
        else if (dig.L1 == 1 && dig.R1 == 0)
        {
            turnLeft(speed,delay);
            reflectance_digital(&dig); 
        }
        
        /* followed two if elses are made for bonus of line follower task, 
           variable bonus set to 1 if task follow the line  */
        else if (bonus && miss && dig.R1 == 0 && dig.L1 == 0)
        {
            print_mqtt(MISS_TOPIC, " %i", xTaskGetTickCount());   
            miss = false;
        }
        else if (bonus && !miss && dig.R1 == 1 && dig.L1 == 1)
        {
            print_mqtt(LINE_TOPIC, " %i", xTaskGetTickCount());
            miss = true;
        }
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
        
    }
    motor_forward(0,0);
}

void driveThruMaze(uint8 speed, uint32 delay){
    
    // declarations
    struct sensors_ dig;
    reflectance_digital(&dig);
    
    //drives forward when all sensors are on the line
    while(sense(dig)!= 12)
    {
        if(dig.L1 == 1 && dig.R1 == 0)
        {
            turnLeft(speed,delay);
            reflectance_digital(&dig);
        }
        if(dig.L1 == 0 && dig.R1 == 1)
        {
            turnRight(speed,delay);
            reflectance_digital(&dig);
        }
            motor_forward(speed,delay);
            reflectance_digital(&dig);
    }
    while(dig.L3 == 0 || dig.R3 == 0)
    {
            if(dig.L2 == 1 && dig.R2 == 0)
            {
                turnLeft(speed,delay);
                reflectance_digital(&dig); 
            }
            if(dig.L2 == 0 && dig.R2 == 1)
            {
                turnRight(speed,delay);
                reflectance_digital(&dig); 
            }
       
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
void turnLeft(uint8_t speed, uint32_t delay) {
    SetMotors(1, 0, speed, speed, delay);
}

void turnRight(uint8_t speed, uint32_t delay) {
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
        correction= (degree * -1) % 360;
    } else{
        rightState=1;
        correction= degree % 360;
    }
    uint32 delay = (correction * 1048)/360;
    
    SetMotors(leftState,rightState, 100, 100, delay);
}

void end(void) {
    printf("End of the programm\n");
    motor_forward(0,0);         
    motor_stop();
}

void end_mqtt(uint32_t start, uint32_t stop) {
    motor_forward(0,0);         
    motor_stop();
    uint32_t elapsed = stop-start;
    print_mqtt(STOP_TOPIC, " %lu", stop);
    print_mqtt(TIME_TOPIC, "elapsed: %lums, %02lus", elapsed, elapsed/1000);
}



// function to start assets. 1 as parameter starts motor, infrared, reflectance, button in same order 
void startUp(int motor, int IR, int reflectance, int button, int ultra) {
    printf("\n\nBoot!\n");
    if(motor == 1){
        motor_start();  
        motor_forward(0,0);
    }
    if(IR == 1){
        IR_Start();
//        printf("to start press IR send\n");
        IR_flush();    
    }
    if(reflectance == 1){
        reflectance_start(); 
        reflectance_set_threshold(13000, 11000, 15000, 15000, 11000, 13000);
    }
    if(button == 1) {
        while(SW1_Read());
    }
    if(ultra == 1) {
        Ultra_Start();
    }
    
    // more to add here
}

uint16_t sense(struct sensors_ dig)
{                               
//returns this value   1          2         4           8           16          32
    uint16_t sum = dig.L3*1 + dig.L2*2 + dig.L1*4 + dig.R1*8 + dig.R2*16 + dig.R3*32;
    
 return sum;   
}


/* [] END OF FILE */
