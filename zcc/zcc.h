#ifndef _ZCC_ZCC_H
#define _ZCC_ZCC_H

#include "type.h"

class Pos;

class Token {
public:
	Token(int _type) :type(_type) { }
	Token(int _type, int _id) :type(_type), id(_id) {  }
	Token(int ty, std::string &_sval) : type(ty), sval(_sval) {  }
	Token(int ty, char _c) : type(ty), ch(_c) {  }

	Token(const Token &t):type(t.type), pos(t.pos),counter(t.counter), id(t.id),
		sval(t.sval), ch(t.ch){}
	Token operator=(const Token &t) {
		type = t.type; 
		pos = t.pos;
		counter = t.counter;
		id = t.id;
		sval = t.sval;
		ch = t.ch;
	}

private:
	int type;
	Pos pos;
	int counter;

	// TKEYWORD
	int id;

	// TSTRING 
	std::string sval;

	// TCHAR
	int ch;
};


class Node {

};


class Type {

};


class Pos {
public:
	Pos() :line(1), cols(1) {  }
	Pos(int _line, int _cols) :line(_line), cols(_cols) {  }
	Pos(const Pos &p) : line(p.line), cols(p.cols) {  }
	Pos operator=(const Pos &p) { line = p.line; cols = p.cols; }
	
	int line;
	int cols;
};


#endif // !_ZCC_ZCC_H