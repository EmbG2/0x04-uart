/*
 * File:   newmainXC16.c
 * Author: lenovo
 *
 * Created on March 13, 2025, 4:14 PM
 */


#include "xc.h"
#include "timer.h"

#define FP 40000000 
#define BAUDRATE 9600
#define BRGVAL (FP / BAUDRATE) / 16 - 1

//void algorithm(){
//    tmr_wait_ms(TIMER2, 7);
//}

int main(void) {
    // Deactivate analog ports
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    // Set LEDs as outputs
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    // Set port RPI20 as input
    TRISEbits.TRISE8 = 1;
    
    // transmit with the port 64 and receive with the port 75
    // assign UART to the appropriate pins
    RPOR0bits.RP64R     = 5;
    RPINR19bits.U2RXR   = 75;
    
    // configure UART peripheral
    U2MODEbits.STSEL    = 1;        // stop bit
    U2MODEbits.PDSEL    = 1;        // parity
    U2MODEbits.ABAUD    = 0;        // auto-baud
    U2MODEbits.BRGH     = 0;        // speed mode
    U2BRG               = BRGVAL;   // BAUD Rate Setting for 9600
    
    U2MODEbits.UARTEN   = 1;        // enable UART
    U2STAbits.UTXEN     = 1;        // enable u1tx
    while(1){
        U2TXREG             = 'C';
    }
    
//    tmr_setup_period(TIMER1, 10);
//    while (1){
//        algorithm();
//        //...
//        ret = tmr_wait_period(TIMER1);
//    }
    return 0;
}

 void __attribute__((__interrupt__)) _U2TXInterrupt(void)
 {
 IFS1bits.U2TXIF = 0; // clear TX interrupt flag
 U2TXREG = 'a'; // Transmit one character
 }
 