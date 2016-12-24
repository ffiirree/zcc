#ifndef _ZCC_FILE_H
#define _ZCC_FILE_H

#include <iostream>
#include <fstream>
#include <memory>
#include "type.h"

class File {
public:
    File() = default;
	File(const std::string &_filename);

	File(const File &f) :filename(f.filename), buffer(f.buffer), ptr(f.ptr), pos(f.pos) {  }
	File &operator=(const File &f) { filename = f.filename; buffer = f.buffer; ptr = f.ptr; pos = f.pos; return *this; }
	~File() {}

	File open(const std::string &_file);

	char next();
    void back(char c);
	char peek();
    char peek2();
    bool test(char c);
    bool test2(char c);
    bool next_is(char c);
	
	Pos getPos() { return pos; }
	inline std::string getFileName() { return filename; }

private:
	void create(const std::string &_filename);

	std::string filename;
	std::shared_ptr<char> buffer;
	char* ptr;
	Pos pos;
};

#endif // !_ZCC_FILE_H
