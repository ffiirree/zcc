#include "lex.h"
#include "type.h"
#include "error.h"


Lex::Lex(const std::string &filename):
	f(filename)
{
#define keyword(ty, name, _) keywords.push_back(std::string(name));
    KEYWORD_MAP
#undef keyword
        scan(filename);
}

void Lex::scan(const std::string &filename)
{
	f.open(filename);

    int counter = 0;
	do {
        last = readToken();
        if (last.getType() != BACKLASH) {
            last.setPos(f.getPos());
            last.setCounter(++counter);
            tokens.push_back(last);
        }
	} while (last.getType() != K_EOF);

	index = 0;
}

Token Lex::readToken()
{
	char c;
	do {
		c = f.next();

		switch (c) {
        case '\n': if (last.getType() != BACKLASH) return Token(TNEWLINE, 0);
        case '\\': return Token(BACKLASH, 0);
        //case ' ': return Token(KEYWORD, static_cast<int>(c));
		case '\r': break;
        case '#': return read_op('#', DS, '#');
		case '+': return read_op2('+', OP_INC, '=', OP_A_ADD, '+');
		case '*': return read_op('=', OP_A_MUL, '*');
		case '%': return read_op('=', OP_A_MOD, '%');
		case '|': return read_op2('|', OP_LOGOR, '=', OP_A_OR, '|');
		case '&': return read_op2('&', OP_LOGAND, '=', OP_A_AND, '&');
		case '^': return read_op('=', OP_A_XOR, '^');
		case '=': return read_op('=', OP_EQ, '=');
		case '!': return read_op('=', OP_NE, '!');
		case '"': return read_string();
		case '\'': return read_char();

		case_a_z: case_A_Z: case '_': return read_id(c);
		case_0_9: return read_num(c);

		case '/':
			if (f.next_is('/')) {
				while (c != '\n' && c != 0)
					c = f.next();
				break;
			}
			else if (f.next_is('*')) {
				while ((!(c == '*' && f.next_is('/'))) && c != 0)
					c = f.next();
				break;
			}
			return read_op('=', OP_A_DIV, '/');

		case '-':
			if (f.next_is('=')) return Token(KEYWORD, OP_A_SUB);
			if (f.next_is('-')) return Token(KEYWORD, OP_DEC);
			if (f.next_is('>')) return Token(KEYWORD, OP_ARROW);
			return Token(KEYWORD, (int)'-');

		case '>':
			if (f.next_is('>'))
				return read_op('=', OP_A_SAR, OP_SAR);
			return read_op('=', OP_GE, '>');
		case '<':
			if (f.next_is('<'))
				return read_op('=', OP_A_SAL, OP_SAL);
			return read_op('=', OP_LE, '<');

		case '.':
			if (f.next_is('.'))
				if (f.next_is('.'))
					return Token(KEYWORD, ELLIPSIS);
			return Token(KEYWORD, (int)'.');

		case '[': case ']': case '(': case ')': case '{':case '}':
		case '?': case ':': case ',': case '~': case ';':
			return Token(KEYWORD, (int)c);
		}
	} while (c != 0);

	return Token(K_EOF, 0);
}


Token Lex::read_op2(char exp1, int _k1, char exp2, int _k2, int _else)
{
	if (f.next_is(exp1))
		return Token(KEYWORD, _k1);
	return Token(KEYWORD, (int)(f.next_is(exp2) ? _k2 : _else));
}

Token Lex::read_op(char exp, int _k, int _else)
{
	return Token(KEYWORD, (int)(f.next_is(exp) ? _k : _else));
}


/**
 * @berif 检查一个标识符是否是关键字
 * @param[in] word:标识符
 * @ret ID or 关键字的枚举常量值
 */
int Lex::isKeyword(std::string &word)
{
	for (size_t i = 0; i < keywords.size(); ++i) {
		if (keywords.at(i) == word) {
			return i + K_AUTO;
		}
	}
	return ID;
}

/**
 * @berif 读取字符串
 * @param[in] fir: 无效
 */
Token Lex::read_string()
{

	std::string s;
    char c = f.next();

    while (c != '"')
    {
        s.push_back(c);
        c = f.next();
    }

	return Token(STRING_, s);
}


/**
 * 注意强制转换为char
 */
Token Lex::read_char()
{
	char c = f.next();
	int r = (c == '\\') ? read_escaped_char() : c;
	c = f.next();
	if (c != '\'')
		error("unterminated char");

	return Token(CHAR_, (char)r);
}

/**
 * @berif 读取标识符，并且检查标识符是不是关键字
 * @param[in] fir: 首字符
 */
Token Lex::read_id(char fir)
{
	int i = 0;
	int c = fir;
	std::string str;

	do {
		str.push_back(c);
		c = f.next();
	} while (isalnum(c) || c == '_');
	f.back(c);

    if (last.to_string() != "#") {                  // 滤掉预处理中的#if等
        if ((i = isKeyword(str)) != ID) {
            return Token(KEYWORD, i);
        }
    }

	return Token(ID, str);
}

/**
 * @attention
 */
Token Lex::read_num(char fir)
{
	char c = fir;
	int type = INTEGER;
	int pos = 0;
	std::string str;

	if (c > '0' || (f.peek() == '.')) {
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

/**
 * escape-sequence = simple-escape-sequence
 *                 | octal-escape-sequence
 *                 | hexadecimal-escape-sequence
 *                 | universal-character-name
 */
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
	case_0_7: return read_octal_char(c);
	}
	error("unknown escape character: \\%c", c);
	return c;
}


/**
 * @berif 读取八进制数据
 */
int Lex::read_octal_char(int c) {
	int r = c - '0';
	if (!(f.peek() >= '0' && f.peek() <= '7'))
		return r;
	r = (r << 3) | (f.next() - '0');
	if (!(f.peek() >= '0' && f.peek() <= '7'))
		return r;
	return (r << 3) | (f.next() - '0');
}

/**
 * @berif 十六进制
 */
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

Token &Lex::next()
{
    if (index == tokens.size())
        error("end of token sequence.");

	return tokens.at(index++);
}

void Lex::back()
{
    if (index == 0)
        error("begin of token sequence.");
	index--;
}


Token Lex::peek()
{
	Token t = next();
	back();
	return t;
}


Token Lex::peek2()
{
    next();
    Token t = next();
    back();
    back();
    return t;
}


bool Lex::next_is(const char e)
{
    if (tokens.at(index).getType() == KEYWORD
        && tokens.at(index).getId() == e) {
        index++;
        return true;
    }

    return false;
}

bool Lex::test(int _id)
{
    Token tok = peek();

    if (tok.getType() == KEYWORD && tok.getId() == _id)
        return true;
    return false;
}

bool Lex::test2(int _id)
{
    Token tok = peek2();

    if (tok.getType() == KEYWORD && tok.getId() == _id)
        return true;
    return false;
}

bool Lex::expect(const char id)
{
    if (next_is(id))
        return true;
    errorp(tokens.at(index).getPos(), "expect : %c", id);
    return false;
}


void Lex::insertFront(Lex &l)
{
    for (size_t i = l.size(); i > 0; --i) {
        tokens.insert(tokens.begin(), l.at(i - 1));
    }
}
void Lex::insertBack(Lex &l)
{
    for (size_t i = 0; i < l.size(); ++i) {
        tokens.push_back(l.at(i));
    }
}

void Lex::insert(Lex &l) {
    for (size_t i = l.size(); i > 0; --i) {
        tokens.insert(tokens.begin() + index, l.at(i - 1));
    }
}