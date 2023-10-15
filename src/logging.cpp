#include "logging.h"

#include <iostream>

// info
void log_i(const std::string& message) { std::cout << "   INFO] " << message << '\n'; }

void log_i(const source_location& loc, const std::string& message)
{
    std::cout << "   INFO -- " << loc.file_name << ":" << loc.line << ":" << loc.column << "] " << message
              << '\n';
}

// warning
void log_w(const std::string& message) { std::cout << "WARNING] " << message << '\n'; }

void log_w(const source_location& loc, const std::string& message)
{
    std::cout << "WARNING -- " << loc.file_name << ":" << loc.line << ":" << loc.column << "] " << message
              << '\n';
}

// error
void log_e(const std::string& message) { std::cout << "  ERROR] " << message << '\n'; }

void log_e(const source_location& loc, const std::string& message)
{
    std::cout << "  ERROR -- " << loc.file_name << ":" << loc.line << ":" << loc.column << "] " << message
              << '\n';
}

// fatal
void log_f(const std::string& message) { std::cout << "  FATAL] " << message << '\n'; }

void log_f(const source_location& loc, const std::string& message)
{
    std::cout << "  FATAL -- " << loc.file_name << ":" << loc.line << ":" << loc.column << "] " << message
              << '\n';
}