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


void progEndEvg(uint32_t delay);
void tankTLEvg(uint8_t speed, uint32_t delay);
void tankTREvg(uint8_t speed, uint32_t delay);
void tankTurnEvg(uint8_t l_speed, uint8_t r_speed, uint32_t delay);
int randomEvg(int min, int max);
void week3_1_evg(void);
void week3_2_evg(void);
void week3_3_evg(void);
void week4_1_evg(void);
void week4_2_evg(void);

#endif

/* [] END OF FILE */
