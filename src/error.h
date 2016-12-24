#ifndef _ZCC_ERROR_H
#define _ZCC_ERROR_H

//#include "zcc.h"

#define _DEBUG_

void error(const std::string &strerr);
void errorp(const Pos &pos, const std::string &strerr);
void warn(const std::string &strerr);
void warnp(const Pos &pos, const std::string &strerr);

#endif // !_ZCC_ERROR_H
