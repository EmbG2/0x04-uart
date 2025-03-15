/*
 * File:   newmainXC16.c
 * Author: lenovo
 *
 * Created on March 13, 2025, 4:14 PM
 */


#include "xc.h"
//#include "timer.h"

#define FCY 72000000 
#define BAUDRATE 9600
#define BRGVAL (FCY / BAUDRATE) / 16 - 1

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
    LATAbits.LATA0 = 1;
    
    // transmit with the port 64 and receive with the port 75
    // assign UART to the appropriate pins
    RPOR0bits.RP64R     = 1;
    RPINR18bits.U1RXR   = 75;
    
    // configure UART peripheral
    U1MODEbits.UARTEN   = 0;
    U1MODEbits.STSEL    = 1;        // stop bit
    U1MODEbits.PDSEL    = 0;        // parity
    U1MODEbits.ABAUD    = 0;        // auto-baud
    U1MODEbits.BRGH     = 0;        // speed mode
    U1BRG               = BRGVAL;   // BAUD Rate Setting for 9600
    
    IEC0bits.U1RXIE = 1;
    IFS0bits.U1RXIF = 0;
    
    U1MODEbits.UARTEN   = 1;        // enable UART
    U1STAbits.UTXEN     = 1;        // enable u1tx
    
    U1TXREG             = 'C';
    
//    tmr_setup_period(TIMER1, 10);
//    tmr_turn(TIMER1, 1);
//    while(1){
//        algorithm();
//        
//        ret = tmr_wait_period(TIMER1);
//    }
    
    return 0;
}

 void __attribute__((__interrupt__)) _U1RXInterrupt(void)
 {
    LATAbits.LATA0 ^= 1;
    char rec = U1RXREG;
    if (U1STAbits.OERR) {
        U1STAbits.OERR = 0; 
    }
    IFS0bits.U1RXIF = 0;
    U1TXREG = 'P';
 }
 