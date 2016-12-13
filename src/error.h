#ifndef _ZCC_ERROR_H
#define _ZCC_ERROR_H

#include "zcc.h"

#define _DEBUG_

#define _log_(fmt,...) do{\
	printf("[%s](%s): #%d \t", __FILE__, __func__, __LINE__);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
}while (0)

#define warning(fmt,...) do{\
    printf("Warning. ");\
	printf("%s: #%d ", __FILE__, __LINE__);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
}while (0)

#define error(fmt,...) do{\
	printf("Error. %s: #%d ", __FILE__, __LINE__);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
    exit(-1);\
}while (0)

#define errorp(pos, fmt,...) do{\
    printf("Error.  ");\
	printf("%d, %d : ", pos.line, pos.cols);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
    exit(-1);\
}while (0)

#define errort(tok, fmt, ...)  do{\
	printf("Error. %s: #%d ", __FILE__, __LINE__);\
	printf("%s: %d: %d", print_filename(tok), tok->line, tok->cols);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
}while (0)

#define warnt(tok, fmt,...) do{\
	printf("%d,%d ", tok->line, tok->cols);\
	printf("Warning. %s: #%d ", __FILE__, __LINE__);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
}while (0)

#define warnp(pos, fmt,...) do{\
    printf("Warning. ");\
	printf("%d,%d ", pos.line, pos.cols);\
	printf(fmt, __VA_ARGS__);\
	printf("\n");\
}while (0)


#define print_token(token) do{\
	if(token->kind != FILE_EOF){\
		printf("Kind = %d", token->kind);\
		if(token->kind == INTEGER || token->kind == ST_STRING \
			|| token->kind == ID || token->kind == FLOAT_NUM){\
			printf("\t%s", token->sval);\
		} else if(token->kind == KEYWORD){\
			if(token->id > 127 && token->id < 165){\
				printf("\t%s", keyword[token->id - 128]);\
			} else if(token->id < 128){\
				printf("\t%c", (char)token->id);\
			}\
		}\
		printf("\n");\
	}\
}while(0)


#endif // !_ZCC_ERROR_H
