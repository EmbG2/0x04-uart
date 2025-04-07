#include "xc.h"
#include "timer.h"
#include "uart.h"
#include "buffer.h"

CircularBuffer main_buffer_1;
CircularBuffer main_buffer_2;
CircularBuffer sec_buffer_1;
CircularBuffer sec_buffer_2;

char *patterns[] = {"LD1", "LD2"};

int flag_a = 0;
int flag_b = 0;

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    TRISEbits.TRISE8 = 1;
    TRISEbits.TRISE9 = 1;

    RPINR0bits.INT1R    = 88;
    INTCON2bits.INT1EP  = 0;
    IFS1bits.INT1IF     = 0;
    IEC1bits.INT1IE     = 1;

    RPINR1bits.INT2R    = 89;
    INTCON2bits.INT2EP  = 0;
    IFS1bits.INT2IF     = 0;
    IEC1bits.INT2IE     = 1;

    RPOR0bits.RP64R     = 1;
    RPINR18bits.U1RXR   = 75;

    buffer_init(&main_buffer_1, patterns, 2);
    buffer_init(&sec_buffer_1, patterns, 2);
    buffer_init(&main_buffer_2, patterns, 2);
    buffer_init(&sec_buffer_2, patterns, 2);

    uart_config(URT1, 1, 0);

    U1TXREG = 'S';
//    while(1){
//        IEC0bits.U1RXIE = 0;
//        detect_pattern(&main_buffer_1);
//        IEC0bits.U1RXIE = 1;
//        
//        if (main_buffer_1.flags[0]) {
//            LATAbits.LATA0 ^= 1;
//            main_buffer_1.flags[0] = 0;
//        }
//
//        if (main_buffer_1.flags[1]) {
//            LATGbits.LATG9 ^= 1;
//            main_buffer_1.flags[1] = 0;
//        }
//    }

    while (1) {
        char received;
        if (buffer_read(&main_buffer_1, &received)) {
            while (U1STAbits.UTXBF);  // Wait if transmit buffer is full
            U1TXREG = received;       // Echo back received character
        }
    }
    return 0;
}
