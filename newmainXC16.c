#include "xc.h"
#include "timer.h"
#include "uart.h"
#include "buffer.h"

int a = 0;
int blink_enabled = 1;
int char_count = 0;
int missed_deadlines = 0;
int send_message[2] = {0, 0};
char uart_receive_buffer[BUFFER_SIZE];

CircularBuffer main_buffer_1;
CircularBuffer main_buffer_2;
CircularBuffer sec_buffer_1;
CircularBuffer sec_buffer_2;

char *patterns[] = {"LD1", "LD2"};

int flag_a = 0;
int flag_b = 0;

void algorithm();

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
//    tmr_setup_period(TIMER1, 10);
//    tmr_setup_period(TIMER3, 10);
//    tmr_turn(TIMER1, 1);

    while(1){
        while (main_buffer_1.count > 0) {
            detect_pattern(&main_buffer_1);
        }

        if (main_buffer_1.flags[0]) {
            LATAbits.LATA0 ^= 1;
            main_buffer_1.flags[0] = 0;
        }

        if (main_buffer_1.flags[1]) {
            LATGbits.LATG9 ^= 1;
            main_buffer_1.flags[1] = 0;
        }

//        a++;
    }

    return 0;
}

void __attribute__((__interrupt__, auto_psv)) _INT1Interrupt(void) {
    IFS1bits.INT1IF = 0;
    IEC1bits.INT1IE = 0;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;
    tmr_turn(TIMER3, 1);
}

void __attribute__((__interrupt__, auto_psv)) _INT2Interrupt(void) {
    IFS1bits.INT2IF = 0;
    IEC1bits.INT2IE = 0;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;
    tmr_turn(TIMER3, 1);
}

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;
    tmr_turn(TIMER3, 0);
    IEC0bits.T3IE = 0;

    if (!IEC1bits.INT1IE && PORTEbits.RE8 == 1) {
        send_message[0] = 1;
        IFS1bits.INT1IF = 0;
        IEC1bits.INT1IE = 1;
    }

    if (!IEC1bits.INT2IE && PORTEbits.RE9 == 1) {
        send_message[1] = 1;
        IFS1bits.INT2IF = 0;
        IEC1bits.INT2IE = 1;
    }
}

void algorithm(){
    tmr_wait_ms_3(TIMER2, 11);
    tmr_turn(TIMER2, 0);
}
