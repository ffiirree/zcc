#ifndef ZCC_LOGGING_H
#define ZCC_LOGGING_H

#include "source-location.h"

// info
void log_i(const std::string& message);

void log_i(const source_location& loc, const std::string& message);

// warning
void log_w(const std::string& message);

void log_w(const source_location& loc, const std::string& message);

// error
void log_e(const std::string& message);

void log_e(const source_location& loc, const std::string& message);

// fatal
void log_f(const std::string& message);

void log_f(const source_location& loc, const std::string& message);

#endif //! ZCC_LOGGING_H