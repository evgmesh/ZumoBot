/*******************************************************************************
* File Name: Refl_led.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Refl_led_H) /* Pins Refl_led_H */
#define CY_PINS_Refl_led_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Refl_led_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Refl_led__PORT == 15 && ((Refl_led__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Refl_led_Write(uint8 value);
void    Refl_led_SetDriveMode(uint8 mode);
uint8   Refl_led_ReadDataReg(void);
uint8   Refl_led_Read(void);
void    Refl_led_SetInterruptMode(uint16 position, uint16 mode);
uint8   Refl_led_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Refl_led_SetDriveMode() function.
     *  @{
     */
        #define Refl_led_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Refl_led_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Refl_led_DM_RES_UP          PIN_DM_RES_UP
        #define Refl_led_DM_RES_DWN         PIN_DM_RES_DWN
        #define Refl_led_DM_OD_LO           PIN_DM_OD_LO
        #define Refl_led_DM_OD_HI           PIN_DM_OD_HI
        #define Refl_led_DM_STRONG          PIN_DM_STRONG
        #define Refl_led_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Refl_led_MASK               Refl_led__MASK
#define Refl_led_SHIFT              Refl_led__SHIFT
#define Refl_led_WIDTH              1u

/* Interrupt constants */
#if defined(Refl_led__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Refl_led_SetInterruptMode() function.
     *  @{
     */
        #define Refl_led_INTR_NONE      (uint16)(0x0000u)
        #define Refl_led_INTR_RISING    (uint16)(0x0001u)
        #define Refl_led_INTR_FALLING   (uint16)(0x0002u)
        #define Refl_led_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Refl_led_INTR_MASK      (0x01u) 
#endif /* (Refl_led__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Refl_led_PS                     (* (reg8 *) Refl_led__PS)
/* Data Register */
#define Refl_led_DR                     (* (reg8 *) Refl_led__DR)
/* Port Number */
#define Refl_led_PRT_NUM                (* (reg8 *) Refl_led__PRT) 
/* Connect to Analog Globals */                                                  
#define Refl_led_AG                     (* (reg8 *) Refl_led__AG)                       
/* Analog MUX bux enable */
#define Refl_led_AMUX                   (* (reg8 *) Refl_led__AMUX) 
/* Bidirectional Enable */                                                        
#define Refl_led_BIE                    (* (reg8 *) Refl_led__BIE)
/* Bit-mask for Aliased Register Access */
#define Refl_led_BIT_MASK               (* (reg8 *) Refl_led__BIT_MASK)
/* Bypass Enable */
#define Refl_led_BYP                    (* (reg8 *) Refl_led__BYP)
/* Port wide control signals */                                                   
#define Refl_led_CTL                    (* (reg8 *) Refl_led__CTL)
/* Drive Modes */
#define Refl_led_DM0                    (* (reg8 *) Refl_led__DM0) 
#define Refl_led_DM1                    (* (reg8 *) Refl_led__DM1)
#define Refl_led_DM2                    (* (reg8 *) Refl_led__DM2) 
/* Input Buffer Disable Override */
#define Refl_led_INP_DIS                (* (reg8 *) Refl_led__INP_DIS)
/* LCD Common or Segment Drive */
#define Refl_led_LCD_COM_SEG            (* (reg8 *) Refl_led__LCD_COM_SEG)
/* Enable Segment LCD */
#define Refl_led_LCD_EN                 (* (reg8 *) Refl_led__LCD_EN)
/* Slew Rate Control */
#define Refl_led_SLW                    (* (reg8 *) Refl_led__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Refl_led_PRTDSI__CAPS_SEL       (* (reg8 *) Refl_led__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Refl_led_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Refl_led__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Refl_led_PRTDSI__OE_SEL0        (* (reg8 *) Refl_led__PRTDSI__OE_SEL0) 
#define Refl_led_PRTDSI__OE_SEL1        (* (reg8 *) Refl_led__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Refl_led_PRTDSI__OUT_SEL0       (* (reg8 *) Refl_led__PRTDSI__OUT_SEL0) 
#define Refl_led_PRTDSI__OUT_SEL1       (* (reg8 *) Refl_led__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Refl_led_PRTDSI__SYNC_OUT       (* (reg8 *) Refl_led__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Refl_led__SIO_CFG)
    #define Refl_led_SIO_HYST_EN        (* (reg8 *) Refl_led__SIO_HYST_EN)
    #define Refl_led_SIO_REG_HIFREQ     (* (reg8 *) Refl_led__SIO_REG_HIFREQ)
    #define Refl_led_SIO_CFG            (* (reg8 *) Refl_led__SIO_CFG)
    #define Refl_led_SIO_DIFF           (* (reg8 *) Refl_led__SIO_DIFF)
#endif /* (Refl_led__SIO_CFG) */

/* Interrupt Registers */
#if defined(Refl_led__INTSTAT)
    #define Refl_led_INTSTAT            (* (reg8 *) Refl_led__INTSTAT)
    #define Refl_led_SNAP               (* (reg8 *) Refl_led__SNAP)
    
	#define Refl_led_0_INTTYPE_REG 		(* (reg8 *) Refl_led__0__INTTYPE)
#endif /* (Refl_led__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Refl_led_H */


/* [] END OF FILE */
