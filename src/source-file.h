#ifndef ZCC_SOURCE_FILE_H
#define ZCC_SOURCE_FILE_H

#include "source-location.h"
#include "type.h"

#include <fstream>
#include <iostream>
#include <memory>

class source_file
{
public:
    source_file() = default;
    explicit source_file(const std::string& filename);

    source_file(const source_file&)            = delete;
    source_file& operator=(const source_file&) = delete;

    // read whole file to a string
    static std::string read(const std::string& filename);

    // open a source file
    void open(const std::string& filename);

    // return next character of source file
    char next();

    void back(char c);

    char peek();

    char peek2();

    bool test(char c);

    bool test2(char c);

    bool next_is(char c);

    // return current location
    source_location location() const { return location_; }

private:
    std::string buffer_{};
    size_t index_{ 0 };

    source_location location_{};
};

#endif // !ZCC_SOURCE_FILE_H
