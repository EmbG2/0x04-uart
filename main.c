/*
 * File:   newmainXC16.c
 * Author: lenovo
 *
 * Created on March 13, 2025, 4:14 PM
 */

#include "xc.h"
#include "uart.h"  // Include this for UART functions
#include "timer.h"

#define TIMER1 1
#define TIMER2 2

int main(void) {
    // Deactivate analog ports
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    tmr_setup(TIMER1, 20);

    // Set LEDs as outputs
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    
    // Set port RPI20 as input
    TRISEbits.TRISE8 = 1;
    
    // Initialize UARTs and send data
    UART_Init(UART_1);
    tmr_start(TIMER1);

    while (1){
        UART_SendChar(UART_1, 'C');
        tmr_wait_period(TIMER1);
        UART_SendChar(UART_1, 'A');
    }
}

void __attribute__((__interrupt__)) _U1RXInterrupt(void)
{
    LATAbits.LATA0 ^= 1;
    char rec = U1RXREG;
    if (U1STAbits.OERR) {
        U1STAbits.OERR= 0;
    }
    IFS0bits.U1RXIF = 0;
    U1TXREG = 'P';
}