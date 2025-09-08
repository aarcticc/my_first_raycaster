#ifndef LOG_UTILS_H
#define LOG_UTILS_H

// Include for size_t type definition used in string operations
#include "deps.h"

// Module identifiers for logging
#define LOG_MODULE_SYSTEM    "[System]"
#define LOG_MODULE_SDL       "[SDL]"
#define LOG_MODULE_GRAPHICS  "[Graphics]"
#define LOG_MODULE_TEXTURE   "[Texture]"
#define LOG_MODULE_MAP       "[Map]"
#define LOG_MODULE_ENEMY     "[Enemy]"
#define LOG_MODULE_PLAYER    "[Player]"
#define LOG_MODULE_INPUT     "[Input]"
#define LOG_MODULE_CONFIG    "[Config]"
#define LOG_MODULE_RENDER    "[Render]"
#define LOG_MODULE_ASSET     "[Asset]"
#define LOG_MODULE_RESOURCE  "[Resource]"

// Log sections for better organization
#define LOG_SECTION_INIT     "INITIALIZATION"
#define LOG_SECTION_GAME     "GAME LOOP"
#define LOG_SECTION_SHUTDOWN "SHUTDOWN SEQUENCE"
#define LOG_SECTION_ERROR    "ERROR"

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
void log_separator(const char* section, const char* subsection);

// New function to log messages with module identifier
// Parameters:
//   module  - the module identifier for the log message
//   message  - the message to be logged
void log_message(const char *module, const char *message);

#endif