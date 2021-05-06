
#include "evgenii.h"

 //********************************Sumo***************************************//

void sumo_wrestling(void) 
{
    
   //decraration of variables
    struct sensors_ dig;
    int32_t startTime = 0, stopTime = 0, obst = 0;
    
    //start of motor, IR and Reflectance sensors. Also setup of button
   //zumo is waiting for button press to start programm
    startUp(MOTOR,INFRA,REFLECT,BUTTON,ULTRA);
    
    reflectance_digital(&dig); 
    driveForward(FAST,0);
    print_mqtt(READY_TOPIC, " zumo");
    
    start_mqtt(&startTime);
    
    motor_forward(100,200);
    
    while(SW1_Read() == RELEASED)
    {
        int angle = randomEvg(45, 180);
        if (Ultra_GetDistance()<3)
        {
            obst = xTaskGetTickCount();
            print_mqtt(OBST_TOPIC, " % lu", obst);
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


//********************************Line follower***************************************//
                               

void line_follower(void) {
    
    // Choose speed (0-255). default speed is maximum
   int speed = FAST;
    
    // Decraration of variables
   int lines=0;
   struct sensors_ dig;
   uint32_t startTime = 0, stopTime = 0;
   
    /* Start of motor, IR and Reflectance sensors. Also setup of button
    zumo is waiting for button press to start programm */
   startUp(MOTOR,INFRA,REFLECT,BUTTON,0);
   
   // Zumo is waiting for IR command and then registers time
   print_mqtt(READY_TOPIC, " zumo");
    
    while(lines<3)
    {
        // Zumo drives across line until begining of next line
        reflectance_digital(&dig);
        driveForward(speed,0);
        lines++;
        
        // On the first line zumo stops and wait for IR command
        if (lines == 1)
        {
            motor_forward(0,0);
            print_mqtt(READY_TOPIC, " line");
            IR_flush();
            IR_wait(); 
            
            // Register start time
            startTime = xTaskGetTickCount();
            print_mqtt(START_TOPIC, " %i", startTime);
        }
    }
    
    // Register stop time, total time and end of the program
    end_mqtt(startTime, stopTime);
}

//********************************Maze***************************************//

void maze(void) {

    // Decraration of variables
    struct sensors_ dig;
    uint32_t startTime = 0, stopTime = 0;
    int Y = -1, X = 0, safeDist = 13;
    int dir = 0, left = -90, right = 90;
    
    /* Start of motor, IR and Reflectance sensors. Also setup of button
    zumo is waiting for button press to start programm */
    startUp(MOTOR,INFRA,REFLECT,BUTTON,ULTRA);
    
    // Zumo get to first line and inform that is ready
    driveForward(FAST,0);
    print_mqtt(READY_TOPIC, " maze");
    printf("Press IR send to start\n");
    // Zumo is waiting for IR command and then registers time
    start_mqtt(&startTime);

    // Find path in maze until last line with no obstacles
    while(Y < 11)
    {
        // Zumo drives forward if no obstacles or not last line
        while((dir == FORWARD) && noRestrict(&Y))
        {
            oneStepForward(&X,&Y);
        }
            // If obstacle detected while driving forward - turn LEFT
        if((dir == FORWARD) && Ultra_GetDistance() < safeDist && !edge(&X))
        {
            dir += LEFT;
            leftInMaze();
        }
            // If obstacle detected while driving forward AND it is edge - turn RIGHT
        if((dir == FORWARD) && Ultra_GetDistance() < safeDist && edge(&X))
        {
            dir += RIGHT;
            rightInMaze();
        }
            // If zumo turned LEFT - drive one intersection and turn UP
        if((dir == LEFT) && noRestrict(&Y) && !edge(&X))
        {   
            dir += RIGHT;
            oneStepLeft(&X,&Y);
            rightInMaze();
            
            // If obstacle detected and it is no edge - turn LEFT again
            if(Ultra_GetDistance() < safeDist && !edge(&X))
            {
                dir += LEFT;
                tankTurnEvg(left);
            }
               // If obstacle detected and it is edge - make 180 degree turn RIGHT
            else if(Ultra_GetDistance() < safeDist && edge(&X))
            {
                dir += RIGHT;
                tankTurnEvg(right);
            }
        }
            // If zumo turned LEFT and obstacle detected make 180 degree turn RIGHT
        if((dir == LEFT) && Ultra_GetDistance() < safeDist)
        {
            dir += RIGHT*2;
            tankTurnEvg(180);
        }
            // If zumo turned RIGHT and it is no edge - drive one intersection and turn UP
        if((dir == RIGHT) && noRestrict(&Y) && X<3)
        {
            dir = dir + LEFT;
            oneStepRight(&X,&Y);
            leftInMaze();
            
            // If obstacle detected after turn UP - turn RIGHT
            if(Ultra_GetDistance() < safeDist)
            {
                dir += RIGHT;
                tankTurnEvg(right);
            }
        }
    } 
        // End of the maze when Y is 11. If not on the central line - navigate to this line
    // If zumo in left side of maze - turn RIGHT and drive until reaches the central line
    if(X<0)
    {
        rightInMaze();
        while(X!=0)
        {
            oneStepRight(&X,&Y);
        }
        motor_forward(200,110);
        tankTurnEvg(left);
    } 
        // If zumo in right side of maze - turn LEFT and drive until reaches the central line
    else if (X>0)
    {
        leftInMaze();
        while(X!=0)
        {
            oneStepLeft(&X,&Y);
        }
        rightInMaze();
    }
        // Then drive forward until the last intersection at coordinate 13
    while(Y < 13)
    {
        oneStepForward(&X,&Y);
    }
    
    // Get out of the maze and mqtt time spend for the completing the maze
    motor_forward(0,0);
    motor_forward(200,400);
    end_mqtt(startTime, stopTime);
}


//****************************************FUNCTIONS***********************************************/

void driveForward(uint8 speed, uint32 delay){
    
    // Declarations
    struct sensors_ dig;
    bool miss = true, bonus = true;
    
    // Drives forward when all sensors are on the line
    reflectance_digital(&dig);
    while(sense(dig)==63)
    {
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
    }
    // Condition when zumo on intersection line
    while(sense(dig)!=63 && sense(dig)!=60 && sense(dig)!= 15)
    {
        // If loosing line from left side of the bot - turn right
        if(dig.L1 == 0 && dig.R1 == 1)
        {
            turnRight(speed,delay);
            reflectance_digital(&dig); 
        } 
        // If loosing line from right side of the bot - turn left
        else if (dig.L1 == 1 && dig.R1 == 0)
        {
            turnLeft(speed,delay);
            reflectance_digital(&dig); 
        }
        /* followed two if elses statements are made for bonus of line follower task, 
           variable bool bonus set to 1 if task "follow the line"  */
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
    
    // Declarations
    struct sensors_ dig;
    reflectance_digital(&dig);
    
    // Drives forward when all sensors are on the line
     while(sense(dig)!=12)
    {
        while(dig.L1 == 0 && dig.R1 == 1)
        {
            tankTurnEvg(2);
            reflectance_digital(&dig); 
        } 
        while(dig.L1 == 1 && dig.R1 == 0)
        {
            tankTurnEvg(-2);
            reflectance_digital(&dig); 
        }
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
    }
    // Navigation of the zumo when on the edges in maze
    while (dig.L3 == 0 && dig.R3 == 0)
    {
        // When on right edge
        if(dig.L2 == 1 && dig.R2 ==0)
        {
            while(dig.L2 == 1 && dig.R2 ==0)
            {
                tankTurnEvg(-1);
                reflectance_digital(&dig);
            }
            // Stabilization
            tankTurnEvg(-4);
        }
        // When on left edge
        if(dig.L2 == 0 && dig.R2 ==1)
        {
            while(dig.L2 == 0 && dig.R2 ==1)
            {
                tankTurnEvg(1);
                reflectance_digital(&dig);
            }
            // Stabilization
            tankTurnEvg(4);
        }
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
    }
    motor_forward(0,0);
}

// Function to check obstacles and Y coordinate
int noRestrict(int *Y)
{
        if(Ultra_GetDistance()<15)
        {
            return 0;   
        }
        if(*Y>10)
        {
            return 0;   
        }
    return 1;
}

// NOT IN USE BECAUSE OF PROBLEMS WITH PSoC CREATOR
bool obst(void)
{
    int i = Ultra_GetDistance();
    if(i < 15) 
    {
        return true;   
    }
    return false;
}

// Function to check if zumo is on the edge of the maze
bool edge(int *X)
{
    if (*X == 3 || *X == -3)
    {
        return 1;   
    } 
    else 
    {
        return 0;
    }
      
}

// Function to center the zumo and tank turn left (anti clockwise) 90 degrees
void leftInMaze(void)
{
    motor_forward(200,110);
    tankTurnEvg(-90);
    motor_forward(0,0); 
}

// Function to center the zumo and tank turn right (clockwise) 90 degrees
void rightInMaze(void)
{
    motor_forward(200,110);
    tankTurnEvg(90);
    motor_forward(0,0);  
}

// Function to drive forward one intersection UP and increasing Y coordinate by 1
void oneStepForward(int *X, int *Y)
{
    *Y = *Y+1;
    driveThruMaze(SPEED, 0);
    print_mqtt(POSI_TOPIC, " %i %i", *X, *Y); 
}

// Function to drive forward one intersection LEFT and decreasing X coordinate by 1
void oneStepLeft(int *X, int *Y)
{
    driveThruMaze(SPEED,0);
    *X = *X-1;
    print_mqtt(POSI_TOPIC, " %i %i", *X, *Y);   
}

// Function to drive forward one intersection RIGHT and increasing X coordinate by 1
void oneStepRight(int *X, int *Y)
{
    driveThruMaze(SPEED,0);
    *X = *X+1;
    print_mqtt(POSI_TOPIC, " %i %i", *X, *Y);
}

// Function to turn zumo left with specified speed and delay
void turnLeft(uint8_t speed, uint32_t delay) {
    SetMotors(1, 0, speed, speed, delay);
}

// Function to turn zumo right with specified speed and delay
void turnRight(uint8_t speed, uint32_t delay) {
    SetMotors(0, 1, speed, speed, delay);
}

// Function to get random value within specified range given as parameter.
int randomEvg(int min, int max) {
    srand(xTaskGetTickCount());
    int random = (rand() % (max - min + 1)) + min;
    return random;
}

// Function of zumo tank turn. Takes angle degree as parameter. + is clockwise - anti clockwise
void tankTurnEvg(int16 degree){
    
    uint8 leftState, rightState, correction;
    
    if(degree>=0)
    {
        leftState = 0;
    } else{
        leftState = 1;
      }
    if(degree<0) 
    {
        rightState = 0;
        correction = (degree * -1) % 360;
    } else{
        rightState = 1;
        correction = degree % 360;
      }
    uint32 delay = (correction * 1048)/360;
    
    SetMotors(leftState,rightState,100,100,delay);
}

// Function to wait for IR command and start timer when it is given
void start_mqtt(uint32_t *startTime)
{
    IR_flush();
    IR_wait();  
    *startTime = xTaskGetTickCount();
    print_mqtt(START_TOPIC, " %lu", *startTime);
}

// Function to stop motor
void end(void) {
    printf("End of the programm\n");
    motor_forward(0,0);         
    motor_stop();
}

// Function to stop motor and register of stop time and time spend for the task
void end_mqtt(uint32_t start, uint32_t stop) {
    motor_forward(0,0);         
    motor_stop();
    stop = xTaskGetTickCount();
    uint32_t elapsed = stop-start;
    print_mqtt(STOP_TOPIC, " %lu", stop);
    print_mqtt(TIME_TOPIC, "% lu ms, %02lu s", elapsed, elapsed/1000);
}


// function to start assets. 1 as parameter starts motor, infrared, reflectance, button in same order 
void startUp(int motor, int ir, int reflectance, int button, int ultra) {
    printf("\n\n\n\n\nBoot!\n\n\n");
    if(motor == 1){
        motor_start();  
        motor_forward(0,0);
    }
    if(ir == 1){
        IR_Start();
        IR_flush();    
    }
    if(reflectance == 1){
        reflectance_start(); 
        reflectance_set_threshold(15000, 15000, 17000, 17000, 15000, 15000);
    }
    if(button == 1) {
        printf("Press the button\n\n");
        while(SW1_Read());
    }
    if(ultra == 1) {
        Ultra_Start();
    }
}

// Function to quickly check values of the reflectance sensors
uint16_t sense(struct sensors_ dig)
{                               
//returns this value   1          2         4           8           16          32
    uint16_t sum = dig.L3*1 + dig.L2*2 + dig.L1*4 + dig.R1*8 + dig.R2*16 + dig.R3*32;
    
 return sum;   
}

// Function of end of the process
void progEndEvg(uint32_t delay) {
    bool led = false;
    while(true){
     BatteryLed_Write(led^=1);
     vTaskDelay(delay);   
    }
}


/* [] END OF FILE */
