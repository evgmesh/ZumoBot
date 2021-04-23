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
void end_sumo();
/******************************* LINE FOLLOWING ***************************************/
void line_startup(int motor, int IR, int ultra, int reflect, int btn);
void line_onTheMark(void);
void line_motorTurn(int16_t degree);
void end_line(void);






/************************************ Project functions**************************/

void SUMO_DB();
void line_project();


#endif







/* [] END OF FILE */
