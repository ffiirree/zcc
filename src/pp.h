#ifndef _ZCC_PP_H
#define _ZCC_PP_H

#include "zcc.h"
#include <fstream>

class Preprocessor {
public:
	Preprocessor(std::string &filename){ }
	Preprocessor(Lex &l) :lex(l){}
	Preprocessor(const Preprocessor &) = delete;
	Preprocessor operator= (const Preprocessor &) = delete;
	~Preprocessor();

	void read_header_name();
	void group();
	void group_part();

	inline bool next_is(int id);
	void expect(int id);

private:
	Lex lex;
	std::ofstream out;
};


#endif