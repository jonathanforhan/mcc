#pragma once

/// @file Log.h
/// @brief Logging for MCC
///
/// Detailed Log Format
///
/// $level: $filename:$line:$index
/// > some_bad_code.syntax_error();
///                 ^~~~~~~~~~~~~~
///                 exaplanation of error

#include <stdio.h>

#if defined __unix__
#define ANSI_NONE "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_ITALIC "\033[3m"
#define ANSI_UNDERLINED "\033[4m"
#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"
#define ANSI_DEFAULT "\033[39m"
#else
#define ANSI_NONE ""
#define ANSI_BOLD ""
#define ANSI_ITALIC ""
#define ANSI_UNDERLINED ""
#define ANSI_BLACK ""
#define ANSI_RED ""
#define ANSI_GREEN ""
#define ANSI_YELLOW ""
#define ANSI_BLUE ""
#define ANSI_MAGENTA ""
#define ANSI_CYAN ""
#define ANSI_WHITE ""
#define ANSI_DEFAULT ""
#endif

#define LOG_INFO(_FMT, ...) fprintf(stderr, "MCC Info: " _FMT "\n", __VA_ARGS__)
#define LOG_WARNING(_FMT, ...) fprintf(stderr, "MCC Warning: " _FMT "\n", __VA_ARGS__)
#define LOG_ERROR(_FMT, ...) fprintf(stderr, "MCC Error: " _FMT "\n", __VA_ARGS__)
#define LOG_FATAL(_FMT, ...) fprintf(stderr, "MCC Fatal: " _FMT "\n", __VA_ARGS__)

/// @brief Error log struct for easy function passing
typedef struct DetailedLog {
    const char* filename;     ///< name of file with error
    size_t line;              ///< line of error in file
    const char* error;        ///< string of the error, ideally entire line
    size_t bad_index;         ///< the index into the line which begins the error
    size_t error_length;      ///< how many chars does the error stretch
    const char* explaination; ///< explanation of why this is wrong
} DetailedLog;

void LogDetailedError(const DetailedLog* log) {
    size_t i;

    // header
    fprintf(stderr, ANSI_RED ANSI_BOLD "error: " ANSI_NONE "%s:%zu:%zu\n", log->filename, log->line, log->bad_index);

    // error
    fprintf(stderr, "> " ANSI_BOLD "%s\n" ANSI_NONE, log->error);

    // error indicating line
    fprintf(stderr, "  ");
    for (i = 0; i < log->bad_index; i++)
        fputc(' ', stderr);
    fputc('^', stderr);
    for (i = i + 1; i < log->bad_index + log->error_length; i++)
        fputc('~', stderr);
    fputc('\n', stderr);

    // explanation
    fprintf(stderr, ANSI_MAGENTA ANSI_BOLD "  ");
    for (i = 0; i < log->bad_index; i++)
        fputc(' ', stderr);
    fprintf(stderr, "%s\n" ANSI_NONE, log->explaination);
}

void LogDetailedWarning(const DetailedLog* log) {
    size_t i;

    // header
    fprintf(
        stderr, ANSI_YELLOW ANSI_BOLD "warning: " ANSI_NONE "%s:%zu:%zu\n", log->filename, log->line, log->bad_index);

    // error
    fprintf(stderr, "> " ANSI_BOLD "%s\n" ANSI_NONE, log->error);
    // error indicating line
    fprintf(stderr, "  ");
    for (i = 0; i < log->bad_index; i++)
        fputc(' ', stderr);
    fputc('^', stderr);
    for (i = i + 1; i < log->bad_index + log->error_length; i++)
        fputc('~', stderr);
    fputc('\n', stderr);

    // explanation
    fprintf(stderr, ANSI_MAGENTA ANSI_BOLD "  ");
    for (i = 0; i < log->bad_index; i++)
        fputc(' ', stderr);
    fprintf(stderr, "%s\n" ANSI_NONE, log->explaination);
}
