#include "parser.h"

void process_uart_commands(char* buffer, char* commands[], int command_numbers_activations[], int save_indx_letter[], int stop_check[]) {
    int skip_actions = 0;
    if (buffer[0] == '\0') {
        skip_actions = 1;
    }
    if (!skip_actions) {
        int reset_idx = 0;
        while (commands[reset_idx] != 0) {
            stop_check[reset_idx] = 0;
            reset_idx++;
        }

        int indx_buffer = 0;
        while (buffer[indx_buffer] != '\0') {
            int indx_command = 0;
            while (commands[indx_command] != 0) {
                if (stop_check[indx_command]) {
                    indx_command++;
                    continue;
                }

                int command_found = (commands[indx_command][0] != '\0');
                int indx_letter = 0;

                while (command_found && commands[indx_command][indx_letter + save_indx_letter[indx_command]] != '\0') {
                    if (buffer[indx_buffer + indx_letter] == '\0') {
                        save_indx_letter[indx_command] += indx_letter;
                        stop_check[indx_command] = 1;
                        command_found = 0;
                        break;
                    }

                    if (buffer[indx_buffer + indx_letter] != commands[indx_command][indx_letter + save_indx_letter[indx_command]]) {
                        command_found = 0;
                        break;
                    }

                    indx_letter++;
                }

                if (!stop_check[indx_command] && save_indx_letter[indx_command] != '\0') {
                    save_indx_letter[indx_command] = 0;
                    if (!command_found) {
                        indx_command--;
                    }
                }

                if (command_found && !stop_check[indx_command]) {
                    command_numbers_activations[indx_command]++;
                }

                indx_command++;
            }

            indx_buffer++;
        }
    }
}