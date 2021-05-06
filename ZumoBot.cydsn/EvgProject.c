
#include "evgenii.h"

 // Sumo

void sumo_wrestling(void) 
{
    
   //decraration of variables
    struct sensors_ dig;
    int32_t startTime = 0, stopTime = 0, obst = 0;
    
    //start of motor, IR and Reflectance sensors. Also setup of button
   //zumo is waiting for button press to start programm
    startUp(MOTOR,INFRA,REFLECT,BUTTON,ULTRA);
    
    reflectance_digital(&dig); 
    driveForward(255,0);
    print_mqtt(READY_TOPIC, "zumo");
    IR_flush();
    IR_wait();
    motor_forward(100,150);
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


//********************************Project***************************************//
                               

// set value of variable bonus in driveForward to 1 
void line_follower(void) {
   //choose speed (0-255). default speed is maximum
   int speed = FAST;
   //decraration of variables
   int lines=0;
   struct sensors_ dig;
   uint32_t startTime = 0, stopTime = 0;
   
   //start of motor, IR and Reflectance sensors. Also setup of button
   //zumo is waiting for button press to start programm
   startUp(MOTOR,INFRA,REFLECT,BUTTON,0);

   print_mqtt(READY_TOPIC, "zumo");
    
    while(lines<3)
    {
        //zumo drives across line until begining of next line
        reflectance_digital(&dig);
        driveForward(speed,0);
        lines++;
        
        //one the first line zumo stops and wait for IR command
        if (lines == 1)
        {
            motor_forward(0,0);
            print_mqtt(READY_TOPIC, "line");
            IR_flush();
            IR_wait(); 
            //register start time
            startTime = xTaskGetTickCount();
            print_mqtt(START_TOPIC, " %i", startTime);
        }
        
    }
    //register stop time, total time and end of the program
    stopTime = xTaskGetTickCount();
    end_mqtt(startTime, stopTime);
}

void maze(void) {
    struct sensors_ dig;
    
    uint32_t startTime = 0, stopTime = 0;
    int maxX = 3;       int maxY = 11;
    int Y = -1;
    int X = 0;
    int left = -90;     int right = 90;
    int dir = 0;
    
    startUp(MOTOR,INFRA,REFLECT,0,ULTRA);
    IR_wait();              // temp - delete and turn on button in startUp
    
    driveForward(200,0);
    print_mqtt(READY_TOPIC, "maze");
    IR_wait();  
    startTime = xTaskGetTickCount();
    print_mqtt(START_TOPIC, " %lu", startTime);
    
    reflectance_digital(&dig);
    while(Y < maxY)
    {
        while((dir == FORWARD) && noRestrict(&Y))
        {
            reflectance_digital(&dig);
            driveThruMaze(SPEED,0);
            Y++;
            print_mqtt(POSI_TOPIC, "%i %i", X, Y);
        }
        if((dir == FORWARD) && Ultra_GetDistance() < 13 && X != - maxX)
        {
            dir += LEFT;
            motor_forward(200,110);
            tankTurnEvg(left);
            motor_forward(0,0); 
        } 
        if((dir == FORWARD) && Ultra_GetDistance() < 15 && X == - maxX)
        {
            dir += RIGHT;
            motor_forward(200,110);
            tankTurnEvg(right);
            motor_forward(0,0);
        }
        if((dir == LEFT) && noRestrict(&Y) && X != - maxX)
        {   
            dir += RIGHT;
            reflectance_digital(&dig);
            driveThruMaze(SPEED,0);
            X--;
            print_mqtt(POSI_TOPIC, "%i %i", X, Y);
            motor_forward(200,110);
            tankTurnEvg(right);
            motor_forward(0,0);
            if(Ultra_GetDistance() < 13 && X != - maxX)
            {
                dir += LEFT;
                tankTurnEvg(left);
                motor_forward(0,0);
            }
            else if(Ultra_GetDistance() < 15 && X == - maxX)
            {
                dir += RIGHT;
                tankTurnEvg(right);
                motor_forward(0,0);
            }
        }
        if((dir == LEFT) && Ultra_GetDistance() < 15)
        {
            dir += RIGHT*2;
            tankTurnEvg(180);
            motor_forward(0,0);
        }
        if((dir == RIGHT) && noRestrict(&Y) && X<3)
        {
            dir = dir + LEFT;
            driveThruMaze(SPEED,0);
            X++;
            print_mqtt(POSI_TOPIC, "%i %i", X, Y);
            motor_forward(200,110);
            tankTurnEvg(left);
            motor_forward(0,0);
            if(Ultra_GetDistance() < 15)
            {
                dir += RIGHT;
                tankTurnEvg(right);
                motor_forward(0,0);
            }
        }
    } 
    if(Y == maxY)
    {
        if(X<0)
        {
            motor_forward(200,110);
            tankTurnEvg(right);
            while(X!=0)
            {
                driveThruMaze(SPEED, 0);
                X++;
                print_mqtt(POSI_TOPIC, "%i %i", X, Y);
                
            }
            motor_forward(200,110);
            tankTurnEvg(left);
        }
        if (X>0)
        {
            motor_forward(200,110);
            tankTurnEvg(left);
            while(X!=0)
            {
                driveThruMaze(SPEED, 0);
                X--;
                print_mqtt(POSI_TOPIC, "%i %i", X, Y);
            }
            motor_forward(200,110);
            tankTurnEvg(right);
        }
        while(Y < 13)
        {
            Y++;
            driveThruMaze(SPEED, 0);
            print_mqtt(POSI_TOPIC, "%i %i", X, Y);
        }
        motor_forward(0,0);
        motor_forward(100,700);
        print_mqtt(STOP_TOPIC, "END");
    }
    stopTime = xTaskGetTickCount();
    end_mqtt(startTime, stopTime);
}




void driveForward(uint8 speed, uint32 delay){
    
    // declarations
    struct sensors_ dig;
    reflectance_digital(&dig);
    bool miss = true, bonus = true;
    
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
      //  printf("sense is %i\n", sense(dig));
    }
         
    while (dig.L3 == 0 && dig.R3 == 0)
    {
        if(dig.L2 == 1 && dig.R2 ==0)
        {
            while(dig.L2 == 1 && dig.R2 ==0)
            {
                tankTurnEvg(-1);
                reflectance_digital(&dig);
            }
            tankTurnEvg(-4);
        }
        if(dig.L2 == 0 && dig.R2 ==1)
        {
            while(dig.L2 == 0 && dig.R2 ==1)
            {
                tankTurnEvg(1);
                reflectance_digital(&dig);
            }
            tankTurnEvg(4);
        }
        
        motor_forward(speed,delay);
        reflectance_digital(&dig); 
    }
    motor_forward(0,0);
}

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
    
bool obst(void)
{
    int i = Ultra_GetDistance();
    if(i < 15) 
    {
        return true;   
    }
    return false;
}

void leftInMaze(void)
{
    motor_forward(200,110);
    tankTurnEvg(-90);
    motor_forward(0,0); 
}

void rightInMaze(void)
{
    motor_forward(200,110);
    tankTurnEvg(90);
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
void startUp(int motor, int ir, int reflectance, int button, int ultra) {
    printf("\n\n\n\n\nBoot!\n\n\n");
    if(motor == 1){
        motor_start();  
        motor_forward(0,0);
    }
    if(ir == 1){
        IR_Start();
//        printf("to start press IR send\n");
        IR_flush();    
    }
    if(reflectance == 1){
        reflectance_start(); 
        reflectance_set_threshold(15000, 15000, 17000, 17000, 15000, 15000);
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