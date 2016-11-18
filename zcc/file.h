#ifndef _ZCC_FILE_H
#define _ZCC_FILE_H

#include <iostream>
#include <fstream>
#include "zcc.h"

class File {
public:
	File(const std::string &filename);

	File(const File &f) = delete;
	File operator=(const File &f) = delete;
	~File();

	char next();
	char peek();
	void back(char c);


	Pos getPos() { return pos; }

private:
	std::fstream in;
	std::fstream out;

	char* buffer;
	char* ptr;

	Pos pos;
};

#endif // !_ZCC_FILE_H
