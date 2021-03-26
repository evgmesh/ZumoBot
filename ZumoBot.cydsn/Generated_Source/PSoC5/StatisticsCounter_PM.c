/*******************************************************************************
* File Name: StatisticsCounter_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
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

static StatisticsCounter_backupStruct StatisticsCounter_backup;


/*******************************************************************************
* Function Name: StatisticsCounter_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  StatisticsCounter_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void StatisticsCounter_SaveConfig(void) 
{
    #if (!StatisticsCounter_UsingFixedFunction)

        StatisticsCounter_backup.CounterUdb = StatisticsCounter_ReadCounter();

        #if(!StatisticsCounter_ControlRegRemoved)
            StatisticsCounter_backup.CounterControlRegister = StatisticsCounter_ReadControlRegister();
        #endif /* (!StatisticsCounter_ControlRegRemoved) */

    #endif /* (!StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  StatisticsCounter_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void StatisticsCounter_RestoreConfig(void) 
{      
    #if (!StatisticsCounter_UsingFixedFunction)

       StatisticsCounter_WriteCounter(StatisticsCounter_backup.CounterUdb);

        #if(!StatisticsCounter_ControlRegRemoved)
            StatisticsCounter_WriteControlRegister(StatisticsCounter_backup.CounterControlRegister);
        #endif /* (!StatisticsCounter_ControlRegRemoved) */

    #endif /* (!StatisticsCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: StatisticsCounter_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  StatisticsCounter_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void StatisticsCounter_Sleep(void) 
{
    #if(!StatisticsCounter_ControlRegRemoved)
        /* Save Counter's enable state */
        if(StatisticsCounter_CTRL_ENABLE == (StatisticsCounter_CONTROL & StatisticsCounter_CTRL_ENABLE))
        {
            /* Counter is enabled */
            StatisticsCounter_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            StatisticsCounter_backup.CounterEnableState = 0u;
        }
    #else
        StatisticsCounter_backup.CounterEnableState = 1u;
        if(StatisticsCounter_backup.CounterEnableState != 0u)
        {
            StatisticsCounter_backup.CounterEnableState = 0u;
        }
    #endif /* (!StatisticsCounter_ControlRegRemoved) */
    
    StatisticsCounter_Stop();
    StatisticsCounter_SaveConfig();
}


/*******************************************************************************
* Function Name: StatisticsCounter_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  StatisticsCounter_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void StatisticsCounter_Wakeup(void) 
{
    StatisticsCounter_RestoreConfig();
    #if(!StatisticsCounter_ControlRegRemoved)
        if(StatisticsCounter_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            StatisticsCounter_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!StatisticsCounter_ControlRegRemoved) */
    
}


/* [] END OF FILE */
