#ifndef DCM_LOGGER_H_
#define DCM_LOGGER_H_

// This file was generated from "config.h.in" by CMake.
#include "dcm/config.h"

#if DCM_ENABLE_LOG

#include <cstring>  // for strrchr()
#include <string>

// Log levels.
// VERB is similar to DEBUG commonly used by other projects.
// USER is for the users who want to log their own logs but don't want any
// VERB or INFO.
#define DCM_VERB 0
#define DCM_INFO 1
#define DCM_USER 2
#define DCM_WARN 3
#define DCM_ERRO 4

// Default log level.
#ifndef DCM_LOG_LEVEL
#define DCM_LOG_LEVEL DCM_USER
#endif

#define DCM_LOG_FILE_NAME "dcm.log"

namespace dcm {

enum LogMode {
  LOG_FILE        = 1,  // Log to file.
  LOG_CONSOLE     = 2,  // Log to console.
  LOG_FLUSH       = 4,  // Flush on each log.
  LOG_OVERWRITE   = 8,  // Overwrite any existing log file.
};

// Commonly used modes.
const int LOG_CONSOLE_FILE_APPEND = LOG_CONSOLE | LOG_FILE;
const int LOG_CONSOLE_FILE_OVERWRITE = LOG_CONSOLE | LOG_FILE | LOG_OVERWRITE;
const int LOG_FILE_OVERWRITE = LOG_FILE | LOG_OVERWRITE;

// Initialize logger.
// If |dir| is empty, log file will be generated in current directory.
void LogInit(const std::string& dir, int modes);

void Log(int level, const char* file, int line, const char* format, ...);

}  // namespace dcm

// Initialize the logger with a level.
#define DCM_LOG_INIT(dir, modes) dcm::LogInit(dir, modes);

// Definition of _WIN32 & _WIN64:
//   https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2015
#if (defined(_WIN32) || defined(_WIN64))

// See: https://stackoverflow.com/a/8488201
// ISSUE: The last path separator of __FILE__ in a header file becomes "/"
//        instead of "\". The result is that __FILENAME__ will contain a
//        prefix of "dcm/". So don't log from a header file!
#define __FILENAME__ std::strrchr("\\" __FILE__, '\\') + 1

#else

#define __FILENAME__ std::strrchr("/" __FILE__, '/') + 1

#endif  // defined(_WIN32) || defined(_WIN64)

// See: https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html

#if DCM_LOG_LEVEL <= DCM_VERB
#define LOG_VERB(format, ...) \
    dcm::Log(DCM_VERB, __FILENAME__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOG_VERB(format, ...)
#endif

#if DCM_LOG_LEVEL <= DCM_INFO
#define LOG_INFO(format, ...) \
    dcm::Log(DCM_INFO, __FILENAME__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOG_INFO(format, ...)
#endif

#if DCM_LOG_LEVEL <= DCM_USER
#define LOG_USER(format, ...) \
    dcm::Log(DCM_USER, __FILENAME__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOG_INFO(format, ...)
#endif

#if DCM_LOG_LEVEL <= DCM_WARN
#define LOG_WARN(format, ...) \
    dcm::Log(DCM_WARN, __FILENAME__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOG_WARN(format, ...)
#endif

#if DCM_LOG_LEVEL <= DCM_ERRO
#define LOG_ERRO(format, ...) \
    dcm::Log(DCM_ERRO, __FILENAME__, __LINE__, format, ##__VA_ARGS__);
#else
#define LOG_ERRO(format, ...)
#endif

#else  // DCM_ENABLE_LOG == 0

#define DCM_LOG_INIT(dir, modes)

#define LOG_VERB(format, ...)
#define LOG_INFO(format, ...)
#define LOG_USER(format, ...)
#define LOG_WARN(format, ...)
#define LOG_ERRO(format, ...)

#endif  // DCM_ENABLE_LOG

#endif  // DCM_LOGGER_H_
