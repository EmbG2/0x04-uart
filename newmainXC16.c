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

char* commands[] = {
    "LD1", // LED 1
    "LD2", // LED 2
    NULL
}

int command_numbers_activations[] = {0, 0};
int save_indx_letter[] = {0, 0};
bool stop_check[] = {false, false};

int send_message[2] = {0, 0};
char uart_receive_buffer[BUFFER_SIZE];

void algorithm();

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
    
    U1TXREG             = 'G';      // Send 'G' if everything works

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
        
        char_count = (char_count + uart_receive(URT1, uart_receive_buffer)) % 100;

        // Fix this part to use the circular buffer correctly -------------------------

        int reset_idx = 0;
        while (commands[reset_idx] != NULL) {
            stop_check[reset_idx] = false;
            reset_idx++;
        }

        int indx_buffer = 0;
        while (uart_receive_buffer[indx_buffer] != '\0') { // Check starting from each buffer's letter
            int indx_command = 0;
            while (command[indx_command] != NULL){ // Check each command's words
                // Skip if we have to wait the end of the next message
                if (stop_check[indx_command]) {
                    indx_command++;
                    continue;
                }

                bool command_found = command[indx_command][0] != '\0';

                int indx_letter = 0;
                while (command_found && command[indx_command][indx_letter + save_indx_letter[indx_command]] != '\0' ) { // Check each command's letter
                    
                    if (uart_receive_buffer[indx_buffer + indx_letter] == '\0'){
                        save_indx_letter[indx_command] = indx_letter;
                        stop_check[indx_command] = true;
                        command_found = false;
                        break;
                    }

                    if (uart_receive_buffer[indx_buffer + indx_letter] != command[indx_command][indx_letter + save_indx_letter[indx_command]]){ // If one letter doesn't correspond to the pattern
                        command_found = false;
                        break;
                    }

                    // Go for the next command's letter
                    indx_letter++;
                }
                if (!stop_check[indx_command] && save_indx_letter[indx_command] != 0){
                    save_indx_letter[indx_command] = 0;
                }
                if(command_found && !stop_check[indx_command]){
                    command_numbers_activations[indx_command]++;
                }

                // Go for the next command's word
                indx_command++;
            }

            // Go for the next buffer's letter
            indx_buffer++;
        }

        // ------------------------------------ END ------------------------------------

        
        if (send_message[0]){
            char msg[4] = {'C', '=', '0' + (char_count / 10), '0' + (char_count % 10)};
            uart_transmit(URT1, msg, 4);
            send_message[0] = 0; // Reset the flag after sending
        }
        if (send_message[1]){
            char msg[4] = {'D', '=', '0' + (missed_deadlines / 10), '0' + (missed_deadlines % 10)};
            uart_transmit(URT1, msg, 4);
            send_message[1] = 0; // Reset the flag after sending
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
    tmr_wait_ms_3(TIMER2, 7);
    tmr_turn(TIMER2, 0);
}