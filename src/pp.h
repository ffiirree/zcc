#ifndef _ZCC_PP_H
#define _ZCC_PP_H

#include "zcc.h"

class Preprocessor {
public:
	Preprocessor(std::string &filename){ }
	Preprocessor(const Preprocessor &) = delete;
	Preprocessor operator= (const Preprocessor &) = delete;
	~Preprocessor();
private:

};


#endif