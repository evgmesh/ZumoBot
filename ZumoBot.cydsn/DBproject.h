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
#include <stdint.h>

#ifndef ZUMO6PROJECT_H_
#define ZUMO6PROJECT_H_

/*************************************SUMO****************************************/
void sumo_startUp(int motor,int IR, int ultra, int reflet, int btn);
void sumo_along_line(void);
void sumo_onTheMark(void);
void sumo_enterance(void);
int sumo_randDeg(void);
void sumo_motorTurn(int16_t degree);
void end_sumo(uint32_t startTime, uint32_t endTime);
/******************************* LINE FOLLOWING ***************************************/
void line_startup(int motor, int IR, int ultra, int reflect, int btn);
void line_onTheMark(void);
void line_motorTurn(int16_t degree);
void end_line(void);

/******************************* MAZE ***********************************************/

void MazeIII_startup(int motor,int IR, int ultra, int reflect, int btn);    
void MazeIII_onTheMark(void);   
void MazeIII_grid_following(void);    
void MazeIII_Mqtt_print(int x, int y);
int MazeIII_random(void);
void MazeIII_Turn(int16_t degree);
void MazeIII_leftedge(void);
void MazeIII_rightedge(void);
void MazeIII_end_line(uint32_t startTime);






/************************************ Project functions**************************/

void SUMO_DB();
void line_project();
void MazeIII();


#endif







/* [] END OF FILE */
