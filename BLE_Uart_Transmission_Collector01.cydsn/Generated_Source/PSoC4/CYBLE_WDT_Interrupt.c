/*******************************************************************************
* File Name: CYBLE_WDT_Interrupt.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <CYBLE_WDT_Interrupt.h>

#if !defined(CYBLE_WDT_Interrupt__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START CYBLE_WDT_Interrupt_intc` */

/* `#END` */

extern cyisraddress CyRamVectors[CYINT_IRQ_BASE + CY_NUM_INTERRUPTS];

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_Start(void)
{
    /* For all we know the interrupt is active. */
    CYBLE_WDT_Interrupt_Disable();

    /* Set the ISR to point to the CYBLE_WDT_Interrupt Interrupt. */
    CYBLE_WDT_Interrupt_SetVector(&CYBLE_WDT_Interrupt_Interrupt);

    /* Set the priority. */
    CYBLE_WDT_Interrupt_SetPriority((uint8)CYBLE_WDT_Interrupt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    CYBLE_WDT_Interrupt_Enable();
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_StartEx
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    CYBLE_WDT_Interrupt_Disable();

    /* Set the ISR to point to the CYBLE_WDT_Interrupt Interrupt. */
    CYBLE_WDT_Interrupt_SetVector(address);

    /* Set the priority. */
    CYBLE_WDT_Interrupt_SetPriority((uint8)CYBLE_WDT_Interrupt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    CYBLE_WDT_Interrupt_Enable();
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_Stop(void)
{
    /* Disable this interrupt. */
    CYBLE_WDT_Interrupt_Disable();

    /* Set the ISR to point to the passive one. */
    CYBLE_WDT_Interrupt_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for CYBLE_WDT_Interrupt.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(CYBLE_WDT_Interrupt_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START CYBLE_WDT_Interrupt_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling CYBLE_WDT_Interrupt_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use CYBLE_WDT_Interrupt_StartEx instead.
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_SetVector(cyisraddress address)
{
    CyRamVectors[CYINT_IRQ_BASE + CYBLE_WDT_Interrupt__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress CYBLE_WDT_Interrupt_GetVector(void)
{
    return CyRamVectors[CYINT_IRQ_BASE + CYBLE_WDT_Interrupt__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. Note calling CYBLE_WDT_Interrupt_Start
*   or CYBLE_WDT_Interrupt_StartEx will override any effect this method would 
*   have had. This method should only be called after CYBLE_WDT_Interrupt_Start or 
*   CYBLE_WDT_Interrupt_StartEx has been called. To set the initial
*   priority for the component use the cydwr file in the tool.
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 3, 0 being the highest.
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_SetPriority(uint8 priority)
{
	uint8 interruptState;
    uint32 priorityOffset = ((CYBLE_WDT_Interrupt__INTC_NUMBER % 4u) * 8u) + 6u;
    
	interruptState = CyEnterCriticalSection();
    *CYBLE_WDT_Interrupt_INTC_PRIOR = (*CYBLE_WDT_Interrupt_INTC_PRIOR & (uint32)(~CYBLE_WDT_Interrupt__INTC_PRIOR_MASK)) |
                                    ((uint32)priority << priorityOffset);
	CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt. 0 - 3, 0 being the highest.
*
*******************************************************************************/
uint8 CYBLE_WDT_Interrupt_GetPriority(void)
{
    uint32 priority;
	uint32 priorityOffset = ((CYBLE_WDT_Interrupt__INTC_NUMBER % 4u) * 8u) + 6u;

    priority = (*CYBLE_WDT_Interrupt_INTC_PRIOR & CYBLE_WDT_Interrupt__INTC_PRIOR_MASK) >> priorityOffset;

    return (uint8)priority;
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_Enable(void)
{
    /* Enable the general interrupt. */
    *CYBLE_WDT_Interrupt_INTC_SET_EN = CYBLE_WDT_Interrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 CYBLE_WDT_Interrupt_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*CYBLE_WDT_Interrupt_INTC_SET_EN & (uint32)CYBLE_WDT_Interrupt__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_Disable(void)
{
    /* Disable the general interrupt. */
    *CYBLE_WDT_Interrupt_INTC_CLR_EN = CYBLE_WDT_Interrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_SetPending(void)
{
    *CYBLE_WDT_Interrupt_INTC_SET_PD = CYBLE_WDT_Interrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: CYBLE_WDT_Interrupt_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void CYBLE_WDT_Interrupt_ClearPending(void)
{
    *CYBLE_WDT_Interrupt_INTC_CLR_PD = CYBLE_WDT_Interrupt__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
