//! Standart C Library
#include <stdio.h>
#include <string.h>
#include <time.h>

//! Project Headers
#include "log_utils.h"

// Global definition of log_file
char log_file[64];

// Function to generate a log filename based on current date/time
// Format: error_DD-MM-YYYY_HH-MM.txt
void get_log_filename(char *buffer, size_t size) {
    // Get current time as Unix timestamp
    time_t now = time(NULL);
    // Convert timestamp to local time structure
    struct tm *t = localtime(&now);
    
    // Format and write filename to buffer using snprintf for safety
    // %02d ensures numbers are padded with zeros to 2 digits
    // tm_mon is 0-based, so add 1 for human-readable month
    // tm_year is years since 1900, so add 1900 for current year
    snprintf(buffer, size, "log_%02d-%02d-%04d_%02d-%02d.txt",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
             t->tm_hour, t->tm_min);
}

// Function to log a message with module information
void log_message(const char *module, const char *message) {
    // Open file in append mode ("a")
    // This creates the file if it doesn't exist
    FILE *fp = fopen(log_file, "a");
    if (!fp) return;  // Return silently if file can't be opened

    // Get current time for the log entry
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Write timestamped message to file
    // Format: [HH:MM:SS] module_name message
    fprintf(fp, "[%02d:%02d:%02d] %s %s\n",
            t->tm_hour, t->tm_min, t->tm_sec, 
            module ? module : "", message);

    // Close file to save changes and free resources
    fclose(fp);
}

// Function to log a separator line with section and subsection
void log_separator(const char* section, const char* subsection) {
    // Open file in append mode ("a")
    // This creates the file if it doesn't exist
    FILE *fp = fopen(log_file, "a");
    if (!fp) return;  // Return silently if file can't be opened

    // Write separator lines and section/subsection information
    fprintf(fp, "\n%s\n", "===========================================");
    
    if (section && subsection) {
        // If both section and subsection are provided, print them
        fprintf(fp, "    %s - %s\n", section, subsection);
    } else if (section) {
        // If only section is provided, print it
        fprintf(fp, "            %s\n", section);
    }
    
    fprintf(fp, "%s\n\n", "===========================================");

    // Close file to save changes and free resources
    fclose(fp);
}

// Function to log an error message (backward compatibility)
void log_error(const char *filename __attribute__((unused)), const char *message) {
    // Call log_message with NULL module for backward compatibility
    log_message(NULL, message);
}