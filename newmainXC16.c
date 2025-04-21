#include "xc.h"
#include "timer.h"
#include "uart.h"
#include "buffer.h"

CircularBuffer main_buffer_1;
CircularBuffer main_buffer_2;
CircularBuffer sec_buffer_1;
CircularBuffer sec_buffer_2;

char *patterns[] = {"$RATE,5*", "$RATE,0*"};
char c;
int flag_a = 0;
int flag_b = 0;

int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    // Set LEDs as outputs
    TRISAbits.TRISA0 = 0;
    TRISGbits.TRISG9 = 0;

    // Initialize UARTs and send data
    UART_Init(UART_1);
    send_uart_char(UART_1, 'C');
    
    buffer_init(&main_buffer_1, patterns, 2);
    buffer_init(&sec_buffer_1, patterns, 2);
    buffer_init(&main_buffer_2, patterns, 2);
    buffer_init(&sec_buffer_2, patterns, 2);

    U1MODEbits.RXINV = 0;  // Invert RX signal (optional)

    while(1){
        IEC0bits.U1RXIE = 0;
        detect_pattern(&main_buffer_1);
        IEC0bits.U1RXIE = 1;
        
        if (main_buffer_1.flags[0]) {
            LATAbits.LATA0 ^= 1;
            main_buffer_1.flags[0] = 0;
        }

        if (main_buffer_1.flags[1]) {
            LATGbits.LATG9 ^= 1;
            main_buffer_1.flags[1] = 0;
        }
    }

//    while (1){
//         while (buffer_read(&main_buffer_1, &c)) {
//             send_uart_char(UART_1, c);
//             tmr_wait_ms_3(TIMER1, 500);
//         }
//    };
    return 0;
}
