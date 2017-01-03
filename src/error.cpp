#include <iostream>
#include <string>
#include "type.h"
#include "error.h"

void error(const std::string &strerr)
{
    std::cout << "Error. " << __FILE__ << ": #" << __LINE__ << " ";
    std::cout << strerr << "." << std::endl;
    //exit(-1);
}

void errorp(const Pos &pos, const std::string &strerr)
{
    std::cout << "Error. " << pos.line  << ", " << pos.cols << ": ";
    std::cout << strerr << "." << std::endl;
    //exit(-1);
}

void warn(const std::string &strerr)
{
    std::cout << "Warning. " << __FILE__ << ": #" + __LINE__ << " ";
    std::cout << strerr << "." << std::endl;
}

void warnp(const Pos &pos, const std::string &strerr)
{
    std::cout << "Warning. " << pos.line << ", " << pos.cols << " ";
    std::cout << strerr << "." << std::endl;
}

