/*
 * File:   newmainXC16.c
 * Author: lenovo
 *
 * Created on March 13, 2025, 4:14 PM
 */


#include "xc.h"
#include "timer.h"

#define FCY 72000000 
#define BAUDRATE 9600
#define BRGVAL (FCY / BAUDRATE) / 16 - 1

#define BUFFER_SIZE 4

char uart_buffer[BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;
int a = 0;
int char_count = 0;
int missed_deadlines = 0;
int blink_enabled = 1;
int intN = 0;

void algorithm(){
    tmr_wait_ms_3(TIMER2, 7);
    tmr_turn(TIMER2, 0);
}

void uart_setup(int UART_n, int stop, int parity){
    switch (UART_n){
        case 1:
            // symbol configuration
            U1MODEbits.UARTEN   = 0;
            U1MODEbits.STSEL    = stop;        // stop bit
            U1MODEbits.PDSEL    = parity;        // parity
            U1MODEbits.ABAUD    = 0;        // auto-baud
            U1MODEbits.BRGH     = 0;        // speed mode
            U1BRG               = BRGVAL;   // BAUD Rate Setting for 9600
            // interrupts for receiving
            IFS0bits.U1RXIF = 0;
            IEC0bits.U1RXIE = 1;
            // activating transmission
            U1MODEbits.UARTEN   = 1;        // enable UART
            U1STAbits.UTXEN     = 1;        // enable u1tx
            break;
        case 2:
            // symbol configuration
            U2MODEbits.UARTEN   = 0;
            U2MODEbits.STSEL    = stop;        // stop bit
            U2MODEbits.PDSEL    = parity;        // parity
            U2MODEbits.ABAUD    = 0;        // auto-baud
            U2MODEbits.BRGH     = 0;        // speed mode
            U2BRG               = BRGVAL;   // BAUD Rate Setting for 9600
            // interrupts for receiving
            IFS1bits.U2RXIF = 0;
            IEC1bits.U2RXIE = 1;
            // activating transmission
            U2MODEbits.UARTEN   = 1;        // enable UART
            U2STAbits.UTXEN     = 1;        // enable u1tx
            break;
        default: 
            return;
    }
}

int main(void) {
    // LED/BUTTON CONFIGURATION
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    TRISEbits.TRISE8 = 1;
    TRISEbits.TRISE9 = 1;
    
    // UART CONFIGURATION
    // assign to the appropriate pins (TX 64, RX 75)
    RPOR0bits.RP64R     = 1;        // uart 1: 1, uart 2: 3
    RPINR18bits.U1RXR   = 75;
    uart_setup(1, 1, 0);
    
    // something sent to indicate start
    U1TXREG             = 'S'; // TODO remove
    
    // TIMER CONFIGURATION
    tmr_setup_period(TIMER1, 10);
    tmr_setup_period(TIMER3, 10); 
    tmr_turn(TIMER1, 1);            // turn the timer on
    IFS0bits.T1IF       = 0;        // Reset the interrupt's flag
    IEC0bits.T1IE       = 0;        // Activate TIMER1's interrupt
    
    // BUTTON INTERRUPTS
    RPINR0bits.INT1R    = 88;
    INTCON2bits.INT1EP  = 0;        // 1 = Falling edge; 0 = Rising edge
    IFS1bits.INT1IF     = 0;        // Reset the interrupt's flag
    IEC1bits.INT1IE     = 1;        // Activate TIMER2's interrupt
    RPINR1bits.INT2R    = 89;
    INTCON2bits.INT2EP  = 0;        // 1 = Falling edge; 0 = Rising edge
    IFS1bits.INT2IF     = 0;        // Reset the interrupt's flag
    IEC1bits.INT2IE     = 1;        // Activate TIMER2's interrupt
    
    while(1){
        algorithm();
        
        a++;
        if (blink_enabled && a >= 20) {
            a = 0;
            LATGbits.LATG9 ^= 1;
        }
        
        missed_deadlines += tmr_wait_period_3(TIMER1);
    }
    
    return 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void) {
    // TODO check whether to read first or clear-overflow-flag first
    // == Critical Section ==
    char rec = U1RXREG; 
    if (U1STAbits.OERR) {  
        U1STAbits.OERR = 0;
        char_count = 0;
    }
    char_count++;
    IFS0bits.U1RXIF = 0;
    
    // == Non-critical Section ==
    // TODO move to a function in main or something, out of interrupt
    uart_buffer[buffer_tail] = rec;
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    if (buffer_tail >= 3) {
        int i = (buffer_tail - 3 + BUFFER_SIZE) % BUFFER_SIZE;
        if (uart_buffer[i] == 'L' &&
            uart_buffer[(i + 1) % BUFFER_SIZE] == 'D' &&
            uart_buffer[(i + 2) % BUFFER_SIZE] == '1') {
            LATAbits.LATA0 ^= 1;            // toggle LED1
            buffer_head = buffer_tail = 0;  // reset buffer
        }
        if (uart_buffer[i] == 'L' &&
            uart_buffer[(i + 1) % BUFFER_SIZE] == 'D' &&
            uart_buffer[(i + 2) % BUFFER_SIZE] == '2') {
            blink_enabled ^= 1;             // toggle LED2-blinking
            buffer_head = buffer_tail = 0;  // reset buffer
        }
    }
}

void __attribute__((__interrupt__, auto_psv)) _INT1Interrupt(void) {
    IFS1bits.INT1IF = 0;            // Reset the flag of the interrupt
    IFS0bits.T3IF = 0;              // Reset the interrupt's flag
    IEC0bits.T3IE = 1;              // Activate TIMER1's interrupt
    tmr_turn(TIMER3, 1);
    intN = 1;
    
}

void __attribute__((__interrupt__, auto_psv)) _INT2Interrupt(void) {
    IFS1bits.INT2IF = 0;            // Reset the flag of the interrupt
    IFS0bits.T3IF = 0;              // Reset the interrupt's flag
    IEC0bits.T3IE = 1;              // Activate TIMER1's interrupt
    tmr_turn(TIMER3, 1);
    intN = 2;
}

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;              // Reset the flag of the interrupt
    tmr_turn(3, 0);
    switch (intN) {
        case 1: {
            if (PORTEbits.RE8){
                U1TXREG             = 'C';
                U1TXREG             = '=';
                U1TXREG             = '0' + char_count / 10;
                U1TXREG             = '0' + char_count % 10;
            }
            break;
        }
        case 2: {
            if (PORTEbits.RE9){
                U1TXREG             = 'D';
                U1TXREG             = '=';
                U1TXREG             = '0' + missed_deadlines / 10;
                U1TXREG             = '0' + missed_deadlines % 10;
            }
            break;
        }
        default:
            break;
    }
    intN = 0;
    IEC0bits.T3IE = 0;
}
