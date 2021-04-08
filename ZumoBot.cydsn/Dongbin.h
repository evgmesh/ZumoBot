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

#ifndef DONGBIN_H_
#define DONGBIN_H_
    
void progEnd_DB(uint32_t delay);
void tankTL_DB(uint8_t speed, uint32_t delay);
void tankTR_DB(uint8_t speed, uint32_t delay);
void softTR_DB();
void tankTL120_DB();
void tankRandTL_DB(uint32_t delay);
void tankRandTR_DB(uint32_t delay);
int randTurnDeg_DB();
int randTurnLR_DB();
void onYourMark_DB();

/********************* Ex.functions **********************/
void week3_1_DB(void);
void week3_2_DB(void);
void week3_3_DB(void);
void week4_1_DB(void);
void week4_2_DB(void);    
    
#endif


/* [] END OF FILE */
