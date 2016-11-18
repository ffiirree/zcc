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
inline bool operator==(const Pos &p1, const Pos &p2) { return p1.line == p2.line && p1.cols == p2.cols; }

/**
 * 词法单元
 */
class Token {
public:
	Token():type(K_EOF), pos(), counter(0), id(0){}
	Token(int _type, int _id) :type(_type), counter(0), id(_id) {  }
	Token(int ty, std::string &_sval) : type(ty), counter(0), sval(_sval) {  }
	Token(int ty, char _c) : type(ty), pos(), counter(0), ch(_c) {  }
	~Token() {
		if (type == ID || type == STRING_ || type == INTEGER || type == FLOAT)
			sval.~basic_string();
	}

	Token(const Token &t) :type(t.type), pos(t.pos), counter(t.counter) { copyUnion(t); }
	Token operator=(const Token &t) {
		type = t.type; 
		pos = t.pos;
		counter = t.counter;
		copyUnion(t);
		return (*this);
	}

	inline int getType() const { return type; }
	inline Pos getPos() const { return pos; }
	inline int getCounter() const { return counter; }
	inline int getId() const { return id; }
	inline std::string getSval() const { return sval; }
	inline int getCh() const { return ch; }

private:
	void copyUnion(const Token &t);

	int type;
	Pos pos;
	int counter;

	union 
	{
		int id;                  // KEYWORD
		std::string sval;        // STRING 
		int ch;                  // CHAR_
	};
	
};
std::ostream &operator<<(std::ostream & os, const Token & t);
bool operator==(const Token &t1, const Token &t2);
bool operator!=(const Token &t1, const Token &t2);

/**
 * 节点的类型
 */
class Type {
public:

private:
	int type;
	int size;

	bool isUnsig;
	bool isStatic;


};


/**
 * AST's Node
 */
class Node {
public:
	Node(){}
	Node(const Node &n):kind(n.kind){}
	inline Node operator=(const Node &n) { kind = n.kind; return *this; }

private:
	int kind;
	Type type;


};






#endif // !_ZCC_ZCC_H