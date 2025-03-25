#include "buffer.h"
#include <stdio.h>
#include <string.h>

void buffer_init(CircularBuffer *buffer, char **patterns, int pattern_count)
{
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    buffer->patterns = patterns;           // Assign the patterns passed in
    buffer->pattern_count = pattern_count; // Store the number of patterns
}

int buffer_write(CircularBuffer *buffer, char value)
{
    if (buffer->count == MAIN_BUFFER_SIZE)
    {
        return 0; // Buffer full
    }
    buffer->data[buffer->tail] = value;
    buffer->tail = (buffer->tail + 1) % MAIN_BUFFER_SIZE;
    buffer->count++;
    return 1;
}

int buffer_read(CircularBuffer *buffer, char *value)
{
    if (buffer->count == 0)
    {
        return 0; // Buffer empty
    }
    *value = buffer->data[buffer->head];
    buffer->head = (buffer->head + 1) % MAIN_BUFFER_SIZE;
    buffer->count--;
    return 1;
}

int buffer_peek(const CircularBuffer *buffer, int index)
{
    if (index >= buffer->count)
    {
        return -1; // Invalid index
    }
    return buffer->data[(buffer->head + index) % MAIN_BUFFER_SIZE];
}

void detect_pattern(CircularBuffer *buffer)
{
    char temp[10]; // Temporary array to hold characters while processing
    int match_found = 0;

    // Loop while there is data in the buffer
    while (buffer->count > 0)
    {
        // Check for all patterns in the buffer
        for (int i = 0; i < buffer->pattern_count; i++)
        {
            int pattern_len = strlen(buffer->patterns[i]);
            int j = 0;
            int valid_pattern = 1;

            // Print the current pattern being checked
            printf("Checking pattern: '%s'\n", buffer->patterns[i]);

            // Check if current substring matches any of the patterns
            for (; j < pattern_len; j++)
            {
                if (buffer_peek(buffer, j) != buffer->patterns[i][j])
                {
                    valid_pattern = 0;
                    break;
                }
            }

            // If a valid pattern is detected
            if (valid_pattern)
            {
                printf("Pattern '%s' detected\n", buffer->patterns[i]);

                // Move the head forward by the length of the detected pattern
                for (int k = 0; k < pattern_len; k++)
                {
                    buffer_read(buffer, &temp[k]); // Read the detected pattern from buffer
                }

                match_found = 1; // Set flag to indicate a pattern was found
                break;           // Exit loop after detecting the first valid pattern
            }
        }

        // If no valid pattern is found, move the head by 1 (for corrupted data)
        if (!match_found)
        {

            printf("Corrupted command detected, moving head\n");
            buffer_read(buffer, temp); // Increment head in case of no valid pattern
        }

        // Reset match_found flag for the next iteration
        match_found = 0;
    }
}
