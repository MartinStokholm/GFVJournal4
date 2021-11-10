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
#include "project.h"
#include <stdio.h>

CY_ISR_PROTO(ISR_UART_rx_handler);
void handleByteReceived(uint8_t byteReceived);

// Global state variable for switching between functionality in main for loop
uint8_t state = 0; 
int offset = 184; // offset after preload of 204g to get to linear area
float koeficent = 0.549; // The hældning from the graph after making measurement

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    char uartBuffer[256];

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    isr_uart_rx_StartEx(ISR_UART_rx_handler);
    UART_1_Start();
    ADC_SAR_1_Start();
    ADC_SAR_1_StartConvert();
    
    UART_1_PutString("Scale application started\r\n");

    for(;;)
    {
        switch(state)
        {
        case 1:
            {
                /* Place your application code here. */
                if (ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT))
                {
                    uint16_t adcValue = ADC_SAR_1_GetResult16();
                    float result = (adcValue * koeficent)- offset;
                                    
                    snprintf(uartBuffer, sizeof(uartBuffer), "Vaegt i gram: %f \r\n", result);
                    UART_1_PutString(uartBuffer);
                }
                CyDelay(1000);
            }
            break;
        case 2:
            {
                // do nothing   
            }
            break;
            
        default: 
            {} 
            break;
        }
    }
}

CY_ISR(ISR_UART_rx_handler)
{
    uint8_t bytesToRead = UART_1_GetRxBufferSize();
    while (bytesToRead > 0)
    {
        uint8_t byteReceived = UART_1_ReadRxData();
        UART_1_WriteTxData(byteReceived); // echo back
        
        handleByteReceived(byteReceived);
        
        bytesToRead--;
    }
}

void handleByteReceived(uint8_t byteReceived)
{
    switch(byteReceived)
    {
        case 'q' :
            {
                state = 1;
            }
            break;
        case 'w' :
            {
                state = 0;
            }
            break;
        default :
            {
                // nothing
            }
            break;
    }
}

/* [] END OF FILE */
