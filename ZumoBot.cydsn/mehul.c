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
#include "stdlib.h"
 void tank_turn(int16 angle);
 void maze_line(struct sensors_ *sensors);
 struct maze_position { int x; int y; int direction; };

# define STOP_2 2

/**
 * @file    main.c
 * @brief   
 * @details  * Enable global interrupt since Zumo library uses interrupts. *<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/



#if 1
void mazeMehul(void)
{
    // defining constants
    const int up = 0;  // north direction
    const int right = 1;  // east direction
    const int left = -1;  // west direction 
    const int x_limit = 3;
    //const int y_limit = 11;
    const int distance_obstaclelimit = 15;
    
    // defining structure variable
    //struct sensors_ sensors;
    struct maze_position pos = { 0, 0, up};
    
    printf("\n Robort is on the starting line\n");
    //struct sensors_ dig;
    //reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
    
    TickType_t start_time = 0;
    //TickType_t stop = 0;
    start_time = xTaskGetTickCount();
    xTaskGetTickCount();
    reflectance_start();
    IR_Start();
    motor_start();
    motor_forward(0,0);
    
    //maze_line(&sensors);
    print_mqtt("Zumo01/ready", "maze");
    IR_wait();
    print_mqtt("Zumo01/start", "%d", start_time);
    //maze_line(&sensors);
    
    while (pos.y < 11) {
    //reflectance_digital(&dig);
    int distance = Ultra_GetDistance();
    
    if (distance > distance_obstaclelimit){
       // maze_line(&sensors);
        pos.y += 1;
        print_mqtt("Zumo01/position", "%d %d", pos.x, pos.y);
    }
    else
    {
        if (pos.direction == up) {
                if (pos.x == x_limit) {
                motor_forward(255,59);
                SetMotors(!left,!right,100,100,262);
                }
                else if (pos.x == -x_limit) {
                motor_forward(255,231);
                SetMotors(left,right,100,100,262);
                 } 
            }
        if (pos.direction == up) {
                motor_forward(255, 19);
                SetMotors(left,right,100,100,262);
                print_mqtt("Zumo01/position", "%d %d", pos.x, pos.y);
                
                if (pos.direction != left) {
                 motor_forward(255,235);
                 SetMotors(!left,!right,100,100,262);   
                }
                 motor_forward(100, 0);
            }
    
         while (pos.direction != up) {
                
               // maze_line(&sensors);
                pos.x += pos.direction;
                motor_forward(255, 19);
                SetMotors(left,right,100,100,262);
                print_mqtt("Zumo01/position", "%d %d", pos.x, pos.y);
            
                if (pos.direction != up && pos.x == pos.direction*x_limit) {
                    tank_turn(pos.direction*180);
                    pos.direction = pos.direction*-1;
                }
        }
    }
                
                if (pos.y == 11) {
            if (pos.x != 0) {
                SetMotors(!left,!right,100,100,262);
                print_mqtt("Zumo01/position", "%d %d", pos.x, pos.y);
                
                while (pos.x != 0) {
                    //maze_line(&sensors);
                    pos.x += pos.direction;
                }
                 motor_forward(100,25);
                 SetMotors(left,right,100,100,262);
            }
            //maze_line(&sensors);
            //maze_line(&sensors);
            motor_forward(255, 393);
                }
    }
}
                
        
   void tank_turn(int16 angle) {

    uint8 left_wheel = (angle < 0) ? 0 : 1;
    uint8 right_wheel = (angle >= 0) ? 0 : 1;
    
   uint8 angle_corrected = ((angle < 0) ? angle * -1 : angle) % 360;
    
   uint32 delay = (angle_corrected * 1048) / 360; 
    
    SetMotors(left_wheel, right_wheel, 100, 100, delay);
    
} 
    
        
    
            
#endif