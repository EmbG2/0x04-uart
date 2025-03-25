#include <stdio.h>
#include <unistd.h> // For sleep (delay)
#include "buffer.h"

CircularBuffer main_buffer;
CircularBuffer secondary_buffer;

void delay_ms(int ms)
{
    usleep(ms * 1000); // Sleep in milliseconds
}

// Simulate writing commands into the main buffer
void simulate_main_buffer()
{
    printf("Writing commands to main buffer...\n");
    buffer_write(&main_buffer, 'L');
    buffer_write(&main_buffer, 'D');
    buffer_write(&main_buffer, '2');
    buffer_write(&main_buffer, 'B');
    buffer_write(&main_buffer, 'U');
    buffer_write(&main_buffer, 'T');
    buffer_write(&main_buffer, 'N');
    buffer_write(&main_buffer, '1');
    printf("Commands written to main buffer\n");
}

// Simulate reading from main buffer and writing to secondary buffer
void transfer_to_secondary_buffer()
{
    while (main_buffer.count > 0)
    {
        char data;
        if (buffer_read(&main_buffer, &data))
        {
            printf("Reading '%c' from main buffer and writing to secondary buffer\n", data);
            buffer_write(&secondary_buffer, data);
        }
        delay_ms(100); // Small delay between each read and write
    }
}

int main()
{
    buffer_init(&main_buffer);
    buffer_init(&secondary_buffer);

    // Simulate writing commands into the main buffer
    simulate_main_buffer();

    // Add a delay to simulate processing time before reading from main buffer
    delay_ms(1000); // 1 second delay to simulate real-world operation

    // Transfer data from main buffer to secondary buffer
    transfer_to_secondary_buffer();

    // Process the secondary buffer to detect patterns
    while (secondary_buffer.count > 0)
    {
        printf("Processing secondary buffer...\n");
        detect_pattern(&secondary_buffer);
        delay_ms(500); // Simulate delays for each operation
    }

    return 0;
}
