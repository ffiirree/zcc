#ifndef _ZCC_ERROR_H
#define _ZCC_ERROR_H

//#include "zcc.h"

#define _DEBUG_

void error(const std::string &strerr);
void errorp(const Pos &pos, const std::string &strerr);
void warn(const std::string &strerr);
void warnp(const Pos &pos, const std::string &strerr);
//
//#define _log_(fmt,...) do{\
//	printf("[%s](%s): #%d \t", __FILE__, __func__, __LINE__);\
//	printf("\n");\
//}while (0)
//
//#define warning(fmt,...) do{\
//    printf("Warning. ");\
//	printf("%s: #%d ", __FILE__, __LINE__);\
//	printf("\n");\
//}while (0)
//
//#define error(fmt,...) do{\
//	printf("Error. %s: #%d ", __FILE__, __LINE__);\
//	printf("\n");\
//}while (0)
//
//#define errorp(pos, fmt,...) do{\
//    printf("Error.  ");\
//	printf("%d, %d : ", pos.line, pos.cols);\
//	printf("\n");\
//}while (0)
//
//#define errort(tok, fmt, ...)  do{\
//	printf("Error. %s: #%d ", __FILE__, __LINE__);\
//	printf("%s: %d: %d", print_filename(tok), tok->line, tok->cols);\
//	printf("\n");\
//}while (0)
//
//#define warnt(tok, fmt,...) do{\
//	printf("%d,%d ", tok->line, tok->cols);\
//	printf("Warning. %s: #%d ", __FILE__, __LINE__);\
//	printf("\n");\
//}while (0)
//
//#define warnp(pos, fmt,...) do{\
//    printf("Warning. ");\
//	printf("%d,%d ", pos.line, pos.cols);\
//	printf("\n");\
//}while (0)



#endif // !_ZCC_ERROR_H
