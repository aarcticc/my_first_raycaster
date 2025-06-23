#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h> // <-- Add this line for readlink

static FILE* log_file = NULL;

// Helper: get program name from /proc/self/exe or fallback to "program"
static void get_program_name(char* buf, size_t buflen) {
    ssize_t len = readlink("/proc/self/exe", buf, buflen - 1);
    if (len > 0) {
        buf[len] = '\0';
        char* slash = strrchr(buf, '/');
        if (slash) memmove(buf, slash + 1, strlen(slash));
    } else {
        strncpy(buf, "program", buflen);
    }
}

// Helper: count existing logs for this program
static int get_log_count(const char* progname) {
    DIR* dir = opendir("logs");
    if (!dir) return 1;
    int count = 1;
    struct dirent* entry;
    char prefix[128];
    snprintf(prefix, sizeof(prefix), "%s_log", progname);
    while ((entry = readdir(dir))) {
        if (strncmp(entry->d_name, prefix, strlen(prefix)) == 0) {
            ++count;
        }
    }
    closedir(dir);
    return count;
}

// Open log file with required format
static void open_log() {
    if (log_file) return;
    char progname[64];
    get_program_name(progname, sizeof(progname));
    int log_count = get_log_count(progname);

    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char timestr[32];
    strftime(timestr, sizeof(timestr), "%d_%m_%y_%H_%M", tm_info);

    char filename[256];
    snprintf(filename, sizeof(filename), "logs/%s_log%03d_%s.log", progname, log_count, timestr);

    log_file = fopen(filename, "w");
    if (!log_file) {
        // fallback
        log_file = fopen("logs/fallback.log", "w");
    }
}

// Ensure log file is closed at program exit
static void close_log() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

__attribute__((constructor))
static void setup_log_cleanup() {
    atexit(close_log);
}

void log_info(const char* fmt, ...) {
    open_log();
    if (!log_file) return;
    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "[INFO] ");
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);
}

void log_error(const char* fmt, ...) {
    open_log();
    if (!log_file) return;
    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "[ERROR] ");
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);
}
