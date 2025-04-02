#include "xc.h"
#include "uart.h"
#include "buffer.h"

#define UART_OVERWRITE_ON_FULL 0

void uart_config(int URT, int stop_bit, int parity_check){    
    switch (URT){
        case URT1:
            U1MODEbits.UARTEN   = 0;
            U1MODEbits.STSEL    = stop_bit;
            U1MODEbits.PDSEL    = parity_check;
            U1MODEbits.ABAUD    = 0;
            U1MODEbits.BRGH     = 0;
            U1BRG               = BRGVAL;
            IFS0bits.U1RXIF = 0;
            IEC0bits.U1RXIE = 1;
            U1MODEbits.UARTEN   = 1;
            U1STAbits.UTXEN     = 1;
            break;
        case URT2:
            U2MODEbits.UARTEN   = 0;
            U2MODEbits.STSEL    = stop_bit;
            U2MODEbits.PDSEL    = parity_check;
            U2MODEbits.ABAUD    = 0;
            U2MODEbits.BRGH     = 0;
            U2BRG               = BRGVAL;
            IFS1bits.U2RXIF = 0;
            IEC1bits.U2RXIE = 1;
            U2MODEbits.UARTEN   = 1;
            U2STAbits.UTXEN     = 1;
            break;
        default:
            return;
    }
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;
    while (U1STAbits.URXDA) {
#if UART_OVERWRITE_ON_FULL
        while (!buffer_write(&main_buffer_1, U1RXREG)) {
            char tmp;
            buffer_read(&main_buffer_1, &tmp);
        }
#else
        char incoming = U1RXREG;
        if (incoming != '\r' && incoming != '\n') {
            buffer_write(&main_buffer_1, incoming);
        }
#endif
    }
    if (U1STAbits.OERR){
        U1STAbits.OERR = 0;
    }
}

void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void) {  
    IFS1bits.U2RXIF = 0;
    while (U2STAbits.URXDA) {
#if UART_OVERWRITE_ON_FULL
        while (!buffer_write(&main_buffer_2, U2RXREG)) {
            char tmp;
            buffer_read(&main_buffer_2, &tmp);
        }
#else
        buffer_write(&main_buffer_2, U2RXREG);
#endif
    }
    if (U2STAbits.OERR){
        U2STAbits.OERR = 0;
    }
}
