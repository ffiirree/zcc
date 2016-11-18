#include "lex.h"
#include "file.h"
#include "type.h"

#include "error.h"


Lex::Lex(const std::string &filename):
	f(filename), index(0)
{
#define keyword(ty, name, _) keywords.push_back(std::string(name));
	KEYWORD_MAP
#undef keyword

		Token t;
		do {
			t = readToken();
			tokens.push_back(t);
		} while (t.getType() != K_EOF);
}

void Lex::scan(const std::string &filename)
{
	f.open(filename);

	Token t;
	do {
		t = readToken();
		tokens.push_back(t);
	} while (t.getType() != K_EOF);

	index = 0;
}

Token Lex::readToken()
{
	char c;
	do {
		c = f.next();

		switch (c) {
		case '\n': case '\r': break;
		case '#': while (f.next() != '\n'); break;
		case '+': return read_rep2('+', OP_INC, '=', OP_A_ADD, '+');
		case '*': return read_rep('=', OP_A_MUL, '*');
		case '%': return read_rep('=', OP_A_MOD, '%');
		case '|': return read_rep2('|', OP_LOGOR, '=', OP_A_OR, '|');
		case '&': return read_rep2('&', OP_LOGAND, '=', OP_A_AND, '&');
		case '^': return read_rep('=', OP_A_XOR, '^');
		case '=': return read_rep('=', OP_EQ, '=');
		case '!': return read_rep('=', OP_NE, '!');
		case '"': return read_string(c);
		case '\'': return read_char();

		case_a_z: case_A_Z: case '_': return read_id(c);
		case_0_9: return read_num(c);

		case '/':
			if (next_is('/')) {
				while (f.next() != '\n' && f.next() != 0);
				break;
			}
			return read_rep('=', OP_A_DIV, '/');

		case '-':
			if (next_is('=')) return Token(KEYWORD, OP_A_SUB);
			if (next_is('-')) return Token(KEYWORD, OP_DEC);
			if (next_is('>')) return Token(KEYWORD, OP_ARROW);
			return Token(KEYWORD, (int)'-');

		case '>':
			if (next_is('>')) 
				return read_rep('=', OP_A_SAR, OP_SAR);
			return read_rep('=', OP_GE, '>');
		case '<':
			if (next_is('<'))
				return read_rep('=', OP_A_SAL, OP_SAL);
			return read_rep('=', OP_LE, '<');

		case '.':
			if (next_is('.'))
				if (next_is('.'))
					return Token(KEYWORD, ELLIPSIS);
			return Token(KEYWORD, (int)'.');

		case '[': case ']': case '(': case ')': case '{':case '}':
		case '?': case ':': case ',': case '~': case ';':
			return Token(KEYWORD, (int)c);
		}
	} while (c != 0);

	return Token(K_EOF, 0);
}


Token Lex::read_rep2(char exp1, int _k1, char exp2, int _k2, char _else)
{
	if (next_is(exp1))
		return Token(KEYWORD, _k1);
	return Token(KEYWORD, (int)(next_is(exp2) ? _k2 : _else));
}

Token Lex::read_rep(char exp, int _k, char _else)
{
	return Token(KEYWORD, (int)(next_is(exp) ? _k : _else));
}

bool Lex::next_is(char e)
{
	char c = f.next();
	if (c == e)
		return true;
	f.back(c);
	return false;
}

int Lex::isKeyword(std::string &word)
{
	for (int i = 0; i < keywords.size(); ++i) {
		if (keywords.at(i) == word) {
			return i + K_AUTO;
		}
	}
	return ID;
}

Token Lex::read_string(char fir)
{
	int c = fir;
	std::string s;
	int pos = 0;

	c = f.next();
	do {
		pos++;
		s.push_back(c);
		c = f.next();
	} while (c != '"');

	return Token(STRING_, s);
}



Token Lex::read_char()
{
	char c = f.next();
	int r = (c == '\\') ? read_escaped_char() : c;
	c = f.next();
	if (c != '\'')
		error("unterminated char");

	return Token(CHAR_, c);
}

Token Lex::read_id(char fir)
{
	int i = 0;
	int c = fir;
	std::string str;

	do {
		str.push_back(c);
		c = f.next();
	} while (isalnum(c) && c != '_');
	f.back(c);

	if ((i = isKeyword(str)) != ID) {
		return Token(KEYWORD, i);
	}
	

	return Token(ID, str);
}


Token Lex::read_num(char fir)
{
	char c = fir;
	int type = INTEGER;
	int pos = 0;
	std::string str;

	if (c > '0') {
		do {
			str.push_back(c);
			c = f.next();
		} while (isdigit(c));

		if (c == 'e' || c == 'E') goto _e;

		if (c == '.') {
			str.push_back(c);
			c = f.next();
			type = FLOAT;
			if (isdigit(c)) {
				do {
					str.push_back(c);
					c = f.next();
				} while (isdigit(c));
			_e:
				if (c == 'e' || c == 'E') {
					str.push_back(c);
					c = f.next();

					if (!(c == '+' || c == '-' || isdigit(c)))
						error("error number!");

					if (c == '-' || c == '+') {
						str.push_back(c);
						c = f.next();
					}

					if (isdigit(c)) {
						while (isdigit(c)) {
							str.push_back(c);
							c = f.next();
						}
					}
				}
			}
		}
		f.back(c);
	}
	else {
		// 十六进制
		str.push_back(c);
		c = f.next();
		if (c == 'x' || c == 'X') {
			do {
				str.push_back(c);
				c = f.next();
			} while (isxdigit(c));
		}
		// 8进制
		else {
			while (c >= '0' && c <= '7') {
				str.push_back(c);
				c = f.next();
			}
		}
		f.back(c);
	}
	return Token(type, str);
}

int Lex::read_escaped_char() {
	int c = f.next();

	switch (c) {
	case '\'': case '"': case '?': case '\\':
		return c;
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	case 'x': return read_hex_char();
	case 'u': return read_universal_char(4);
	case 'U': return read_universal_char(8);
	case_0_7: return read_octal_char(c);
	}
	error("unknown escape character: \\%c", c);
	return c;
}


// Reads an octal escape sequence.
int Lex::read_octal_char(int c) {
	int r = c - '0';
	if (!nextoct())
		return r;
	r = (r << 3) | (f.next() - '0');
	if (!nextoct())
		return r;
	return (r << 3) | (f.next() - '0');
}

// Reads a \x escape sequence.
int Lex::read_hex_char() {
	int c = f.next();
	if (!isxdigit(c))
		error("\\x is not followed by a hexadecimal character: %c", c);
	int r = 0;
	for (;; c = f.next()) {
		switch (c) {
		case_0_9: r = (r << 4) | (c - '0'); continue;
		case_a_f: r = (r << 4) | (c - 'a' + 10); continue;
		case_A_F: r = (r << 4) | (c - 'A' + 10); continue;
		default: f.back(c); return r;
		}
	}
}

bool Lex::nextoct() {
	int c = f.peek();
	return '0' <= c && c <= '7';
}

// Reads \u or \U escape sequences. len is 4 or 8, respecitvely.
int Lex::read_universal_char(int len) {
	unsigned int r = 0;
	for (int i = 0; i < len; i++) {
		char c = f.next();
		switch (c) {
		case_0_9: r = (r << 4) | (c - '0'); continue;
		case_a_f: r = (r << 4) | (c - 'a' + 10); continue;
		case_A_F: r = (r << 4) | (c - 'A' + 10); continue;
		default: error("invalid universal character: %c", c);
		}
	}
	/*if (!is_valid_ucn(r))
		errorp(p, "invalid universal character: \\%c%0*x", (len == 4) ? 'u' : 'U', len, r);*/
	return r;
}



Token Lex::next()
{
	return tokens.at(index++);
}

void Lex::back()
{
	index--;
}


Token Lex::peek()
{
	Token t = next();
	back();
	return t;
}

void Token::copyUnion(const Token &t)
{
	switch (t.getType()) {
	case KEYWORD: 
	case K_EOF:
		id = t.id; 
		break;

	case CHAR_: 
		ch = t.ch; 
		break;

	case ID: 
	case STRING_:
	case INTEGER:
	case FLOAT:
		new(&sval) std::string(t.sval);
		break;
	}
}

std::ostream &operator<<(std::ostream & os, const Token & t)
{
	os << t.getType() << "\t";

	if (t.getType() == KEYWORD) {
		switch (t.getId())
		{
#define keyword(ty, name, _) case ty: os << name ;break;
#define op(ty,name) case ty: os << name;break;
			KEYWORD_MAP
				OP_MAP
#undef keyword
#undef op
		default: os << (char)t.getId(); break;
		}
		os << std::endl;
	}
	else if(t.getType() == ID)
		os << t.getSval().c_str() << std::endl;
	else if (t.getType() == CHAR_)
		os << (char)t.getCh() << std::endl;
	else if (t.getType() == STRING_ || t.getType() == INTEGER || t.getType() == FLOAT)
		os << t.getSval().c_str() << std::endl;

	return os;
}
bool operator==(const Token &t1, const Token &t2)
{
	return (t1.getType() == t2.getType() && t1.getPos() == t2.getPos() && t1.getCounter() == t2.getCounter() &&
		t1.getCh() == t2.getCh() && t1.getId() == t2.getId() && t1.getSval() == t2.getSval());
}
bool operator!=(const Token &t1, const Token &t2)
{
	return !(t1 == t2);
}

