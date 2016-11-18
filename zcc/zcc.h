#ifndef _ZCC_ZCC_H
#define _ZCC_ZCC_H

#include "type.h"
class Pos {
public:
	Pos() :line(1), cols(1) {  }
	Pos(int _line, int _cols) :line(_line), cols(_cols) {  }
	Pos(const Pos &p) : line(p.line), cols(p.cols) {  }
	Pos operator=(const Pos &p) { line = p.line; cols = p.cols; return (*this); }

	int line;
	int cols;
};


class Token {
public:
	Token():type(K_EOF), pos(), counter(0), id(0), sval(), ch(0){}
	//Token(int _type) :type(_type), pos(), counter(0), id(0), sval(), ch(0) { }
	Token(int _type, int _id) :type(_type), counter(0), id(_id), sval(), ch(0) {  }
	Token(int ty, std::string &_sval) : type(ty), counter(0), id(0), sval(_sval), ch(0) {  }
	// ×Ö·û³£Á¿
	Token(int ty, char _c) : type(ty), pos(), counter(0), id(0), sval(), ch(_c) {  }

	Token(const Token &t):type(t.type), pos(t.pos),counter(t.counter), id(t.id),
		sval(t.sval), ch(t.ch){}
	Token operator=(const Token &t) {
		type = t.type; 
		pos = t.pos;
		counter = t.counter;
		id = t.id;
		sval = t.sval;
		ch = t.ch;
		return (*this);
	}

	inline int getType() const { return type; }
	inline Pos getPos() const { return pos; }
	inline int getCounter() const { return counter; }
	inline int getId() const { return id; }
	inline std::string getSval() const { return sval; }
	inline int getCh() const { return ch; }

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




#endif // !_ZCC_ZCC_H