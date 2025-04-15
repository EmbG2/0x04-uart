/* Microchip Technology Inc. and its subsidiaries... [license left untouched] */

#ifndef PARSER_H
#define	PARSER_H

#ifdef	__cplusplus
extern "C" {
#endif

// Parses the received UART buffer against known command patterns
void process_uart_commands(char* buffer, char* commands[], int command_numbers_activations[], int save_indx_letter[], int stop_check[]);

#ifdef	__cplusplus
}
#endif

#endif	/* PARSER_H */