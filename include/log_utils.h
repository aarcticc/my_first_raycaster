#ifndef LOG_UTILS_H
#define LOG_UTILS_H

// Include for size_t type definition used in string operations
#include <stddef.h>

// Global array to store the log file name
// Size of 64 characters allows for reasonable file path length
// 'extern' keyword makes this variable accessible from other source files
extern char log_file[64];

// Function to generate a log filename based on current date/time
// Parameters:
//   buffer - destination where the generated filename will be stored
//   size   - size of the buffer to prevent buffer overflow
void get_log_filename(char *buffer, size_t size);

// Function to write error messages to the log file
// Parameters:
//   filename - name of the file where the error occurred
//   message  - the error message to be logged
void log_error(const char *filename, const char *message);

// Function to write a separator line to the log file
// Parameters:
//   filename - name of the file where the separator is logged
//   phase    - the game phase to be logged
void log_separator(const char* filename, const char* phase);

#endif