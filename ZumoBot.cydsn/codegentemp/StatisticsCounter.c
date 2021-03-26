/*******************************************************************************
* File Name: StatisticsCounter.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "StatisticsCounter.h"

uint8 StatisticsCounter_initVar = 0u;


/*******************************************************************************
* Function Name: StatisticsCounter_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void StatisticsCounter_Init(void) 
{
        #if (!StatisticsCounter_UsingFixedFunction && !StatisticsCounter_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!StatisticsCounter_UsingFixedFunction && !StatisticsCounter_ControlRegRemoved) */
        
        #if(!StatisticsCounter_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 StatisticsCounter_interruptState;
        #endif /* (!StatisticsCounter_UsingFixedFunction) */
        
        #if (StatisticsCounter_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            StatisticsCounter_CONTROL &= StatisticsCounter_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                StatisticsCounter_CONTROL2 &= ((uint8)(~StatisticsCounter_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                StatisticsCounter_CONTROL3 &= ((uint8)(~StatisticsCounter_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (StatisticsCounter_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                StatisticsCounter_CONTROL |= StatisticsCounter_ONESHOT;
            #endif /* (StatisticsCounter_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            StatisticsCounter_CONTROL2 |= StatisticsCounter_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            StatisticsCounter_RT1 &= ((uint8)(~StatisticsCounter_RT1_MASK));
            StatisticsCounter_RT1 |= StatisticsCounter_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            StatisticsCounter_RT1 &= ((uint8)(~StatisticsCounter_SYNCDSI_MASK));
            StatisticsCounter_RT1 |= StatisticsCounter_SYNCDSI_EN;

        #else
            #if(!StatisticsCounter_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = StatisticsCounter_CONTROL & ((uint8)(~StatisticsCounter_CTRL_CMPMODE_MASK));
            StatisticsCounter_CONTROL = ctrl | StatisticsCounter_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = StatisticsCounter_CONTROL & ((uint8)(~StatisticsCounter_CTRL_CAPMODE_MASK));
            
            #if( 0 != StatisticsCounter_CAPTURE_MODE_CONF)
                StatisticsCounter_CONTROL = ctrl | StatisticsCounter_DEFAULT_CAPTURE_MODE;
            #else
                StatisticsCounter_CONTROL = ctrl;
            #endif /* 0 != StatisticsCounter_CAPTURE_MODE */ 
            
            #endif /* (!StatisticsCounter_ControlRegRemoved) */
        #endif /* (StatisticsCounter_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!StatisticsCounter_UsingFixedFunction)
            StatisticsCounter_ClearFIFO();
        #endif /* (!StatisticsCounter_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        StatisticsCounter_WritePeriod(StatisticsCounter_INIT_PERIOD_VALUE);
        #if (!(StatisticsCounter_UsingFixedFunction && (CY_PSOC5A)))
            StatisticsCounter_WriteCounter(StatisticsCounter_INIT_COUNTER_VALUE);
        #endif /* (!(StatisticsCounter_UsingFixedFunction && (CY_PSOC5A))) */
        StatisticsCounter_SetInterruptMode(StatisticsCounter_INIT_INTERRUPTS_MASK);
        
        #if (!StatisticsCounter_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)StatisticsCounter_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            StatisticsCounter_WriteCompare(StatisticsCounter_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            StatisticsCounter_interruptState = CyEnterCriticalSection();
            
            StatisticsCounter_STATUS_AUX_CTRL |= StatisticsCounter_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(StatisticsCounter_interruptState);
            
        #endif /* (!StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void StatisticsCounter_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (StatisticsCounter_UsingFixedFunction)
        StatisticsCounter_GLOBAL_ENABLE |= StatisticsCounter_BLOCK_EN_MASK;
        StatisticsCounter_GLOBAL_STBY_ENABLE |= StatisticsCounter_BLOCK_STBY_EN_MASK;
    #endif /* (StatisticsCounter_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!StatisticsCounter_ControlRegRemoved || StatisticsCounter_UsingFixedFunction)
        StatisticsCounter_CONTROL |= StatisticsCounter_CTRL_ENABLE;                
    #endif /* (!StatisticsCounter_ControlRegRemoved || StatisticsCounter_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: StatisticsCounter_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  StatisticsCounter_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void StatisticsCounter_Start(void) 
{
    if(StatisticsCounter_initVar == 0u)
    {
        StatisticsCounter_Init();
        
        StatisticsCounter_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    StatisticsCounter_Enable();        
}


/*******************************************************************************
* Function Name: StatisticsCounter_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void StatisticsCounter_Stop(void) 
{
    /* Disable Counter */
    #if(!StatisticsCounter_ControlRegRemoved || StatisticsCounter_UsingFixedFunction)
        StatisticsCounter_CONTROL &= ((uint8)(~StatisticsCounter_CTRL_ENABLE));        
    #endif /* (!StatisticsCounter_ControlRegRemoved || StatisticsCounter_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (StatisticsCounter_UsingFixedFunction)
        StatisticsCounter_GLOBAL_ENABLE &= ((uint8)(~StatisticsCounter_BLOCK_EN_MASK));
        StatisticsCounter_GLOBAL_STBY_ENABLE &= ((uint8)(~StatisticsCounter_BLOCK_STBY_EN_MASK));
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void StatisticsCounter_SetInterruptMode(uint8 interruptsMask) 
{
    StatisticsCounter_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: StatisticsCounter_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   StatisticsCounter_ReadStatusRegister(void) 
{
    return StatisticsCounter_STATUS;
}


#if(!StatisticsCounter_ControlRegRemoved)
/*******************************************************************************
* Function Name: StatisticsCounter_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   StatisticsCounter_ReadControlRegister(void) 
{
    return StatisticsCounter_CONTROL;
}


/*******************************************************************************
* Function Name: StatisticsCounter_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    StatisticsCounter_WriteControlRegister(uint8 control) 
{
    StatisticsCounter_CONTROL = control;
}

#endif  /* (!StatisticsCounter_ControlRegRemoved) */


#if (!(StatisticsCounter_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: StatisticsCounter_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void StatisticsCounter_WriteCounter(uint32 counter) \
                                   
{
    #if(StatisticsCounter_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (StatisticsCounter_GLOBAL_ENABLE & StatisticsCounter_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        StatisticsCounter_GLOBAL_ENABLE |= StatisticsCounter_BLOCK_EN_MASK;
        CY_SET_REG16(StatisticsCounter_COUNTER_LSB_PTR, (uint16)counter);
        StatisticsCounter_GLOBAL_ENABLE &= ((uint8)(~StatisticsCounter_BLOCK_EN_MASK));
    #else
        CY_SET_REG32(StatisticsCounter_COUNTER_LSB_PTR, counter);
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}
#endif /* (!(StatisticsCounter_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: StatisticsCounter_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint32) The present value of the counter.
*
*******************************************************************************/
uint32 StatisticsCounter_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(StatisticsCounter_UsingFixedFunction)
		(void)CY_GET_REG16(StatisticsCounter_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(StatisticsCounter_COUNTER_LSB_PTR_8BIT);
	#endif/* (StatisticsCounter_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(StatisticsCounter_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(StatisticsCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG32(StatisticsCounter_STATICCOUNT_LSB_PTR));
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint32) Present Capture value.
*
*******************************************************************************/
uint32 StatisticsCounter_ReadCapture(void) 
{
    #if(StatisticsCounter_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(StatisticsCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG32(StatisticsCounter_STATICCOUNT_LSB_PTR));
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint32) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void StatisticsCounter_WritePeriod(uint32 period) 
{
    #if(StatisticsCounter_UsingFixedFunction)
        CY_SET_REG16(StatisticsCounter_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG32(StatisticsCounter_PERIOD_LSB_PTR, period);
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint32) Present period value.
*
*******************************************************************************/
uint32 StatisticsCounter_ReadPeriod(void) 
{
    #if(StatisticsCounter_UsingFixedFunction)
        return ((uint32)CY_GET_REG16(StatisticsCounter_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG32(StatisticsCounter_PERIOD_LSB_PTR));
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}


#if (!StatisticsCounter_UsingFixedFunction)
/*******************************************************************************
* Function Name: StatisticsCounter_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void StatisticsCounter_WriteCompare(uint32 compare) \
                                   
{
    #if(StatisticsCounter_UsingFixedFunction)
        CY_SET_REG16(StatisticsCounter_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG32(StatisticsCounter_COMPARE_LSB_PTR, compare);
    #endif /* (StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint32) Present compare value.
*
*******************************************************************************/
uint32 StatisticsCounter_ReadCompare(void) 
{
    return (CY_GET_REG32(StatisticsCounter_COMPARE_LSB_PTR));
}


#if (StatisticsCounter_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: StatisticsCounter_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void StatisticsCounter_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    StatisticsCounter_CONTROL &= ((uint8)(~StatisticsCounter_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    StatisticsCounter_CONTROL |= compareMode;
}
#endif  /* (StatisticsCounter_COMPARE_MODE_SOFTWARE) */


#if (StatisticsCounter_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: StatisticsCounter_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void StatisticsCounter_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    StatisticsCounter_CONTROL &= ((uint8)(~StatisticsCounter_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    StatisticsCounter_CONTROL |= ((uint8)((uint8)captureMode << StatisticsCounter_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (StatisticsCounter_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: StatisticsCounter_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void StatisticsCounter_ClearFIFO(void) 
{

    while(0u != (StatisticsCounter_ReadStatusRegister() & StatisticsCounter_STATUS_FIFONEMP))
    {
        (void)StatisticsCounter_ReadCapture();
    }

}
#endif  /* (!StatisticsCounter_UsingFixedFunction) */


/* [] END OF FILE */

