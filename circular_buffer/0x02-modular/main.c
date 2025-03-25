#include <stdio.h>
#include <unistd.h>
#include "buffer.h"

CircularBuffer main_buffer;
CircularBuffer secondary_buffer;

void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void simulate_main_buffer()
{
    printf("Writing commands to main buffer...\n");
    buffer_write(&main_buffer, 'L');
    buffer_write(&main_buffer, 'D');
    buffer_write(&main_buffer, '1');
    buffer_write(&main_buffer, 'B');
    buffer_write(&main_buffer, 'U');
    buffer_write(&main_buffer, 'T');
    buffer_write(&main_buffer, 'N');
    buffer_write(&main_buffer, '1');
    printf("Commands written to main buffer\n");
}

void transfer_to_secondary_buffer()
{
    printf("Difference before transfer is %d\n", main_buffer.count);

    // Transfer data from main_buffer to secondary_buffer
    while (main_buffer.count > 0)
    { // Keep transferring until buffer is empty
        char data;
        if (buffer_read(&main_buffer, &data))
        {
            printf("Reading '%c' from main buffer and writing to secondary buffer\n", data);
            buffer_write(&secondary_buffer, data);
            // Print the updated count, head, and tail
            printf("Count is %d, Head is %d, Tail is %d\n", main_buffer.count, main_buffer.head, main_buffer.tail);
        }
        delay_ms(100); // Small delay between each read and write
    }

    // Print the difference after the transfer (should be 0 since we processed everything)
    printf("Difference after transfer is %d\n", main_buffer.count);
}

int main()
{
    char *patterns[] = {"LD1", "BUTN1", "CMDX"};

    buffer_init(&main_buffer, patterns, 3);
    buffer_init(&secondary_buffer, patterns, 3);

    // Simulate writing commands into the main buffer
    simulate_main_buffer();

    // Process the main buffer into secondary buffer
    transfer_to_secondary_buffer();

    printf("Processing secondary buffer...\n");
    detect_pattern(&secondary_buffer);

    // Simulate writing new data after processing
    buffer_write(&main_buffer, 'C');
    buffer_write(&main_buffer, 'M');
    buffer_write(&main_buffer, 'D');
    buffer_write(&main_buffer, 'X');
    delay_ms(1000);

    printf("Processing secondary buffer...\n");
    transfer_to_secondary_buffer(); // Transfer newly written data to secondary buffer
    detect_pattern(&secondary_buffer);

    return 0;
}
