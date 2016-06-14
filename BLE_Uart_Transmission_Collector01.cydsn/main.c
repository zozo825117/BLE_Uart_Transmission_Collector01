/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This project demonstrates the operation of the Heart Rate Profile
*  in Collector (Central) role.
*
* Related Document:
*  HEART RATE PROFILE SPECIFICATION v1.0
*  HEART RATE SERVICE SPECIFICATION v1.0
*
*******************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include "common.h"
#include "comm.h"

#define CYBLE_UUID_CUSTOM_UART_TX_SERVICE			0x290A

#define BLE_DISCONNECTED_STATE			0x00
#define BLE_ADV_STATE					0x01
#define BLE_CONNECTED_STATE				0x02
#define BLE_SCAN_STATE					0x03

#define ADV_LED_BLINK_PERIOD			20000
#define CONN_LED_PERIOD					90000


uint32 TimerCnt1ms=0;
uint8 InterruptHpn;
uint8 BleConnected=0;
uint8 ble_state;

void AppCallBack(uint32 event, void* eventParam)
{
	CYBLE_API_RESULT_T apiResult;
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
	uint8 len;


#ifdef DEBUG_OUT    
    DebugOut(event, eventParam);
		//printf("ACB evt: %ld \r\n",event);
#endif
    
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
					printf("EVT_STACK_ON \r\n");
					goto start_scan;
					
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
					BleConnected = 0;
					printf("EVT_GAP_DEVICE_DISCONNECTED \r\n");
					goto start_scan;

        case CYBLE_EVT_TIMEOUT:
					printf("EVT_TIMEOUT \r\n");
start_scan:
						if(BleConnected == 0)
							{
		            StartScan(CYBLE_UUID_CUSTOM_UART_TX_SERVICE);
		            //Scanning_LED_Write(LED_ON);				
		            ble_state = BLE_SCAN_STATE;
							}

            break;
#ifndef DEBUG_OUT
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            	ScanProgressEventHandler((CYBLE_GAPC_ADV_REPORT_T *)eventParam);
            break;

				case CYBLE_EVT_GAPC_SCAN_START_STOP:
						printf("EVT_GAPC_SCAN_START_STOP \r\n");
						if(0u != (flag & CONNECT))
						{
								printf("Connect to the Device: %x \r\n", deviceN);
								/* Connect to selected device */
								apiResult = CyBle_GapcConnectDevice(&peerAddr[deviceN]);
								if(apiResult != CYBLE_ERROR_OK)
								{
										printf("ConnectDevice API Error: %x \r\n", apiResult);
								}

								flag &= ~CONNECT;
								BleConnected = 1;
						}
						break;

				case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF: 
						wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
						if(wrReqParam->handleValPair.attrHandle == UART_TX_HANDLE)
							{
								len = wrReqParam->handleValPair.value.len;
								Buffer[0] = len;
								memcpy(&Buffer[1],wrReqParam->handleValPair.value.val,len);
								uCommState.Bit.BLERxFinshed = ENABLED;
								//printf("len %d buf[0] %d  \r\n", len,Buffer[0]);
							}
								
					break;

					case CYBLE_EVT_GATT_CONNECT_IND:
							printf("EVT_GATT_CONNECT_IND \r\n");
						break;

					case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
							printf("CYBLE_EVT_GATTC_XCHNG_MTU_RSP \r\n");
						break;

#endif
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
						printf("EVT_GAP_DEVICE_CONNECTED \r\n");
						apiResult = CyBle_GattcExchangeMtuReq(cyBle_connHandle,0x200);
						if(apiResult == CYBLE_ERROR_OK)
							{
								printf("zozo test exchg mtu req send\r\n");
							}
						ble_state = BLE_CONNECTED_STATE;


            break;

        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:

            break;

        default:
            break;
    }
}

/*******************************************************************************
* Function Name: HandleLEDs
********************************************************************************
* Summary:
*        Update LED states as per BLE Status
*
* Parameters:
*  state: present BLE state.
*
* Return:
*  void
*
*******************************************************************************/
void HandleLEDs(uint8 state)
{

		static uint16 led_counter = TRUE;
		static uint32 led_conn_counter = TRUE;
		static uint8 on_off_status;
		static uint8 conn_status_update = FALSE;

		switch(state)
		{
			case BLE_CONNECTED_STATE:
				if(conn_status_update)
				{
					conn_status_update = FALSE;
					led_conn_counter = CONN_LED_PERIOD;
				}
				else
				{
					if(led_conn_counter != FALSE)
					{
						if(CONN_LED_PERIOD == led_conn_counter)
						{
							Scanning_LED_Write(LED_ON);
						}
						else if(led_conn_counter == 1)
						{
							Scanning_LED_Write(LED_OFF);

						}
						
						led_conn_counter--;
					}
				}
			break;
			
			case BLE_ADV_STATE:
				if((--led_counter) == FALSE)
				{
					led_counter = ADV_LED_BLINK_PERIOD;
					if(FALSE == on_off_status)
					{
						on_off_status = TRUE;
						Scanning_LED_Write(LED_ON);
		
					}
					else
					{
						on_off_status = FALSE;
						Scanning_LED_Write(LED_OFF);
					}
				}
				
				conn_status_update = TRUE;
			break;
			
			case BLE_SCAN_STATE:
				if((--led_counter) == FALSE)
				{
					led_counter = ADV_LED_BLINK_PERIOD;
					if(FALSE == on_off_status)
					{
						on_off_status = TRUE;
						Scanning_LED_Write(LED_ON);
		
					}
					else
					{
						on_off_status = FALSE;
						Scanning_LED_Write(LED_OFF);
					}
				}
				
				conn_status_update = TRUE;
			break;

			case BLE_DISCONNECTED_STATE:
				Scanning_LED_Write(LED_OFF);
				led_counter = FALSE;
				
				conn_status_update = TRUE;
			break;
			
			default:
			
			break;
		}

}

/*******************************************************************************
* Define Interrupt service routine and allocate an vector to the Interrupt
********************************************************************************/
CY_ISR(InterruptHandler)
{
    /* Check interrupt source and clear Inerrupt */
    InterruptHpn = Timer_GetInterruptSourceMasked();
   	if (InterruptHpn == Timer_INTR_MASK_CC_MATCH)
    {
        Timer_ClearInterrupt(Timer_INTR_MASK_CC_MATCH);      
        //test_1_Write(~ test_1_Read());
    }
    else
    {
        Timer_ClearInterrupt(Timer_INTR_MASK_TC);
        //test_2_Write(~ test_2_Read());
				TimerCnt1ms++;
				
    }
}

int main()
{
    CYBLE_API_RESULT_T apiResult;
  
    CYBLE_LP_MODE_T lpMode;

    CyGlobalIntEnable;
    
    CommInit();               /* Start communication component */
    printf("BLE Uart Transmission Collector Example Project \r\n");
    
    Scanning_LED_Write(LED_OFF);

    apiResult = CyBle_Start(AppCallBack);
    if(apiResult != CYBLE_ERROR_OK)
    {
        printf("CyBle_Start API Error: %xd \r\n", apiResult);
    }
		else
			{
				printf("CyBle_Start API ok \r\n");
			}
    
		/* Enable the Interrupt component connected to interrupt */
		TC_CC_ISR_StartEx(InterruptHandler);
		
		/* Start the components */
		Timer_Start();
    
    while(1)
    {
        if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
        {
            /* Enter DeepSleep mode between connection intervals */
            lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
            if(lpMode == CYBLE_BLESS_DEEPSLEEP) 
            {
                /* Put the device into the Deep Sleep mode only when all debug information has been sent 
			                if(UART_DEB_SpiUartGetTxBufferSize() == 0u)
			                {
			                    CySysPmDeepSleep();
			                }
			                else
			                {
			                    CySysPmSleep();
			                }*/
                CySysPmSleep();
                /* Handle scanning led blinking */
                HandleLEDs(ble_state);
            }
            HandleLEDs(ble_state);
        }
		/***********************************************************************
        * Wait for connection established with Central device
        ***********************************************************************/
        if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {
            /*******************************************************************
            *  Periodically measure a battery level and temperature and send 
            *  results to the Client
            *******************************************************************/    
            CommMonitorUart();
            CommMonitorBLE();
            
            #if 0
            if(mainTimer != 0u)
            {
                mainTimer = 0u;

                if(storeBondingData == ENABLED)
                {
                    cystatus retValue;
                    retValue = CyBle_StoreBondingData(0u);
                    printf("Store bonding data, status: %lx \r\n", retValue);
                    storeBondingData = DISABLED;
                }
    
            }
            #endif
            
            
        }
        
        
        /*******************************************************************
        *  Processes all pending BLE events in the stack
        *******************************************************************/        
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
