#ifndef BUFFER_H
#define BUFFER_H

#define MAIN_BUFFER_SIZE 10
#define SECONDARY_BUFFER_SIZE 5

typedef struct
{
    char data[MAIN_BUFFER_SIZE];
    int head;
    int tail;
    int count;
    char **patterns;   // Array of patterns to detect
    int pattern_count; // Number of patterns
} CircularBuffer;

void buffer_init(CircularBuffer *buffer, char **patterns, int pattern_count);
int buffer_write(CircularBuffer *buffer, char value);
int buffer_read(CircularBuffer *buffer, char *value);
int buffer_peek(const CircularBuffer *buffer, int index);
void detect_pattern(CircularBuffer *buffer);

#endif
