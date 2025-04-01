/*
 * File:   newmainXC16.c
 * Author: lenovo
 *
 * Created on March 13, 2025, 4:14 PM
 */


#include "xc.h"
#include "timer.h"
#include "uart.h"


int a = 0;
int blink_enabled = 1;

int char_count = 0;
int missed_deadlines = 0;

int send_message[2] = {0, 0};

char uart_receive_buffer[BUFFER_SIZE];

void algorithm();

int contains_pattern(const char *buffer, const char *pattern) {
    while (*buffer) {
        const char *b = buffer;
        const char *p = pattern;
        while (*b && *p && (*b == *p)) {
            b++;
            p++;
        }
        if (!*p) {
            return 1; // Pattern found
        }
        buffer++;
    }
    return 0; // Pattern not found
}

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;

    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;
    TRISEbits.TRISE8 = 1;
    TRISEbits.TRISE9 = 1;
    
    // Map the buttons interrupts
    
    RPINR0bits.INT1R    = 88;
    INTCON2bits.INT1EP  = 0;        // 1 = Falling edge; 0 = Rising edge
    IFS1bits.INT1IF     = 0;        // Reset the interrupt's flag
    IEC1bits.INT1IE     = 1;        // Activate TIMER1's interrupt

    RPINR1bits.INT2R    = 89;
    INTCON2bits.INT2EP  = 0;        // 1 = Falling edge; 0 = Rising edge
    IFS1bits.INT2IF     = 0;        // Reset the interrupt's flag
    IEC1bits.INT2IE     = 1;        // Activate TIMER2's interrupt
    
    
    // Assign to the appropriate pins (TX 64, RX 75) to UART 1
    
    RPOR0bits.RP64R     = 1;        // UART 1 -> port 1, UART 2 -> port 3
    RPINR18bits.U1RXR   = 75;
    
    uart_config(URT1, 1, 0);
    
    U1TXREG             = 'S';      // Send 'S' if everything works
    
    // Timers configuration
    
    tmr_setup_period(TIMER1, 10);
    
    tmr_setup_period(TIMER3, 10);   // Timer to remove bouncing effect
    
    tmr_turn(TIMER1, 1);            // Turn TIMER 1 on
    while(1){
        algorithm();                // Wait 7ms
        
        a++;
        if (blink_enabled && a >= 20) {
            a = 0;
            LATGbits.LATG9 ^= 1;
        }
        uart_receive(URT1, uart_receive_buffer);
        
        if (contains_pattern(uart_receive_buffer, "LD1")) {
            LATAbits.LATA0 ^= 1;
        }
        if (contains_pattern(uart_receive_buffer, "LD2")) {
            // Stop or resume LED2 blinking
            blink_enabled = !blink_enabled;
        }
        
        if (send_message[0]){
            char msg[4] = {'C', '=', '0' + (char_count / 10), '0' + (char_count % 10)};
            uart_transmit(URT1, msg, 4);
        }
        if (send_message[1]){
            char msg[4] = {'D', '=', '0' + (missed_deadlines / 10), '0' + (missed_deadlines % 10)};
            uart_transmit(URT1, msg, 4);
        }

        

        missed_deadlines = (missed_deadlines + tmr_wait_period_3(TIMER1)) % 100;
    }
    
    return 0;
}

void __attribute__((__interrupt__, auto_psv)) _INT1Interrupt(void) {
    IFS1bits.INT1IF = 0;  // Clear interrupt flag
    IEC1bits.INT1IE = 0;  // Disable INT1 to prevent re-triggering
    IFS0bits.T3IF = 0;    // Clear Timer3 interrupt flag
    IEC0bits.T3IE = 1;    // Enable Timer3 interrupt
    tmr_turn(TIMER3, 1);  // Start Timer3
}

void __attribute__((__interrupt__, auto_psv)) _INT2Interrupt(void) {
    IFS1bits.INT2IF = 0;  // Clear interrupt flag
    IEC1bits.INT2IE = 0;  // Disable INT2 to prevent re-triggering
    IFS0bits.T3IF = 0;    // Clear Timer3 interrupt flag
    IEC0bits.T3IE = 1;    // Enable Timer3 interrupt
    tmr_turn(TIMER3, 1);  // Start Timer3
}

void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0;   // Clear Timer3 interrupt flag
    tmr_turn(TIMER3, 0); // Stop Timer3
    IEC0bits.T3IE = 0;   // Disable Timer3 interrupt

    if (!IEC1bits.INT1IE && PORTEbits.RE8 == 1) { // If INT1 was disabled & button released
        send_message[0] = 1;  // Notify main loop
        IFS1bits.INT1IF = 0;  // Reset INT1 flag
        IEC1bits.INT1IE = 1;  // Re-enable INT1
    }

    if (!IEC1bits.INT2IE && PORTEbits.RE9 == 1) { // If INT2 was disabled & button released
        send_message[1] = 1;  // Notify main loop
        IFS1bits.INT2IF = 0;  // Reset INT2 flag
        IEC1bits.INT2IE = 1;  // Re-enable INT2
    }
}

void algorithm(){
    tmr_wait_ms_3(TIMER2, 11);
    tmr_turn(TIMER2, 0);
}