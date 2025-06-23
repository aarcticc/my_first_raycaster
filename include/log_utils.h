#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#include <stddef.h>

extern char log_file[64];

void get_log_filename(char *buffer, size_t size);
void log_error(const char *filename, const char *message);

#endif