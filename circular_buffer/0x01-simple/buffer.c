#include "buffer.h"
#include <stdio.h>

void buffer_init(CircularBuffer *buffer)
{
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
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
    char c1, c2, c3, c4, c5;

    // Loop until there is no more data in the buffer
    while (buffer->head != buffer->tail)
    {
        // Peek the first 3 characters to detect pattern 'LD1'
        c1 = buffer_peek(buffer, 0); // Current head
        c2 = buffer_peek(buffer, 1); // Next char
        c3 = buffer_peek(buffer, 2); // Next char

        // Check if 'LD1' is detected
        if (c1 == 'L' && c2 == 'D' && c3 == '1')
        {
            printf("Pattern 'LD1' detected\n");
            // Move the head forward by 3 after detecting 'LD1'
            buffer_read(buffer, &c1); // Read 'L'
            buffer_read(buffer, &c2); // Read 'D'
            buffer_read(buffer, &c3); // Read '1'
            return;                   // Return after detecting the first pattern
        }

        // Peek the next 5 characters to detect pattern 'BUTN1'
        c4 = buffer_peek(buffer, 3); // Next char after 'LD1'
        c5 = buffer_peek(buffer, 4); // Next char after 'BUTN1'

        // Check if 'BUTN1' is detected
        if (c1 == 'B' && c2 == 'U' && c3 == 'T' && c4 == 'N' && c5 == '1')
        {
            printf("Pattern 'BUTN1' detected\n");
            // Move the head forward by 5 after detecting 'BUTN1'
            buffer_read(buffer, &c1); // Read 'B'
            buffer_read(buffer, &c2); // Read 'U'
            buffer_read(buffer, &c3); // Read 'T'
            buffer_read(buffer, &c4); // Read 'N'
            buffer_read(buffer, &c5); // Read '1'
            return;                   // Return after detecting the second pattern
        }

        // If no pattern is detected, move the head and report corrupted data
        printf("Corrupted command detected, moving head\n");
        buffer_read(buffer, &c1); // Increment head in case of no valid pattern
    }
}
