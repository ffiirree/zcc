#ifndef ZCC_SOURCE_LOCATION
#define ZCC_SOURCE_LOCATION

#include <string>

struct source_location
{
    size_t line{ 1 };
    size_t column{ 1 };
    std::string file_name{};
    std::string function_name{};
};

#endif //! ZCC_SOURCE_LOCATION