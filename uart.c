#include "uart.h"

#define FCY 72000000UL 
#define BAUDRATE 9600
#define BRGVAL ((FCY / BAUDRATE) / 16 - 1)  // Corrected parentheses

void UART_Init(unsigned char uart) {
    if (uart == UART_1) {
        // Configure UART1
        U1MODEbits.UARTEN = 0;   // Disable UART1
        RPOR0bits.RP64R = 1;     // TX pin mapping
        RPINR18bits.U1RXR = 75;  // RX pin mapping
        U1MODEbits.UARTEN = 0;   // disable UART
        U1MODEbits.STSEL = 1;    // Stop bit
        U1MODEbits.PDSEL = 0;    // Parity
        U1MODEbits.ABAUD = 0;    // Auto-baud disabled
        U1MODEbits.BRGH = 0;     // Low-speed mode
        U1BRG = BRGVAL;          // Baud rate
        IEC0bits.U1RXIE = 1;
        IFS0bits.U1RXIF =0;
        U1MODEbits.UARTEN = 1;   // Enable UART1
        U1STAbits.UTXEN = 1;     // Enable transmitter
    } else if (uart == UART_2) {
        // Configure UART2
        U2MODEbits.UARTEN = 0;   // Disable UART2
        RPOR0bits.RP64R = 3;     // TX pin mapping
        RPINR18bits.U1RXR = 75;  // RX pin mapping
        U2MODEbits.STSEL = 1;    // Stop bit
        U2MODEbits.PDSEL = 0;    // Parity
        U2MODEbits.ABAUD = 0;    // Auto-baud disabled
        U2MODEbits.BRGH = 0;     // Low-speed mode
        U2BRG = BRGVAL;          // Baud rate
        IEC0bits.U1RXIE = 1;
        IFS0bits.U1RXIF =0;
        U2MODEbits.UARTEN = 1;   // Enable UART2
        U2STAbits.UTXEN = 1;     // Enable transmitter
    }
}

void UART_SendChar(unsigned char uart, char data) {
    if (uart == UART_1) {
        U1TXREG = data;          // Send character
    } else if (uart == UART_2) {
        U2TXREG = data;          // Send character
    }
}
