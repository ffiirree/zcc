#include "lex.h"
#include "type.h"
#include "error.h"

Lex::Lex()
{
#define keyword(ty, name, _) keywords.push_back(std::string(name));
    KEYWORD_MAP
#undef keyword
}

void Lex::scan(const std::string &filename, TokenSequence &ts)
{
	f.open(filename);
    ts.setFileName(filename);

    int counter = 0;
	do {
        last = readToken();
        if (last.getType() != OP_BACKLASH && last.getType() != T_EOF) {
            last.setPos(f.getPos());
            ts.push_back(last);
        }
	} while (last.getType() != T_EOF);
}

Token Lex::readToken()
{
	char c;
	while((c = f.next()) != 0) {
		switch (c) {
        case '\n': if (last.getType() != OP_BACKLASH) return{ T_NEWLINE, 0 };
        case '\\': return{ OP_BACKLASH, 0 };
		case '\r': break;
        case '#':  return f.next_is('#') ? Token(OP_DS) : Token('#');
        case '+':  return f.next_is('+') ? Token(OP_INC) : (f.next_is('=') ? Token(OP_A_ADD) : Token('+'));
		case '*':  return f.next_is('=') ? Token(OP_A_MUL) : Token('*');
		case '%':  return f.next_is('=') ? Token(OP_A_MOD) : Token('%');
		case '|':  return f.next_is('|') ? Token(OP_LOGOR) : (f.next_is('=') ? Token(OP_A_OR) : Token('|'));
		case '&':  return f.next_is('&') ? Token(OP_LOGAND) : (f.next_is('=') ? Token(OP_A_AND) : Token('&'));
		case '^':  return f.next_is('=') ? Token(OP_A_XOR) : Token('^');
		case '=':  return f.next_is('=') ? Token(OP_EQ) : Token('=');
		case '!':  return f.next_is('=') ? Token(OP_NE) : Token('!');
        case '>':  return f.next_is('>') ? (f.next_is('=') ? Token(OP_A_SAR) : Token(OP_SAR)) : (f.next_is('=') ? Token(OP_GE) : Token('>'));
        case '<':  return f.next_is('<') ? (f.next_is('=') ? Token(OP_A_SAL) : Token(OP_SAL)) : (f.next_is('=') ? Token(OP_LE) : Token('<'));
        case '.':  return f.next_is('.') ? (f.next_is('.') ? Token(ELLIPSIS) : false) : Token('.');
		case '"':  return read_string();
		case '\'': return read_char();

		case_a_z: case_A_Z: case '_': return read_id(c);
		case_0_9: return read_num(c);
        _CASE_6('[', ']', '{', '}', '(', ')') : case '?': case ':': case ',': case '~': case ';': return{ c };

		case '/':
			if (f.test('/') || f.test('*')) {
                skipComments();
                break;
			}
			return f.next_is('=') ? Token(OP_A_DIV) : Token('/');

		case '-':
            if (f.next_is('=')) return{ OP_A_SUB };
            if (f.next_is('-')) return{ OP_DEC };
            if (f.next_is('>')) return{ OP_ARROW };
            return{ '-' };
		}
    };

    return{ T_EOF, 0 };
}


void Lex::skipComments()
{
    if (f.next_is('/')) {
        char c = f.next();
        while (c != '\n' && c != 0)
            c = f.next();
    }
    else if (f.next_is('*')) {
        char c = f.next();
        while ((!(c == '*' && f.next_is('/'))) && c != 0)
            c = f.next();
    }
}

/**
 * @berif 
 * @param[in] str:
 * @ret ID or 
 */
int Lex::isKeyword(std::string &str)
{
	for (size_t i = 0; i < keywords.size(); ++i) {
		if (keywords.at(i) == str) {
			return i + K_AUTO;
		}
	}
	return T_IDENTIFIER;
}

/**
 * @berif 
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

    return{ T_STRING, s };
}


/**
 * @berif
 */
Token Lex::read_char()
{
	char c = f.next();
	int r = (c == '\\') ? read_escaped_char() : c;
	c = f.next();
	if (c != '\'')
		error("unterminated char");

    return{ T_CHAR, (char)r };
}

/**
 * @berif 
 * @param[in] fir:
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

    if (last.toString() != "#") {
        if ((i = isKeyword(str)) != T_IDENTIFIER) {
            return{ i };
        }
    }

    return{ T_IDENTIFIER, str };
}

/**
 * @attention
 */
Token Lex::read_num(char fir)
{
	char c = fir;
	int type = T_INTEGER;
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
			type = T_FLOAT;
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
		str.push_back(c);
		c = f.next();
		if (c == 'x' || c == 'X') {
			do {
				str.push_back(c);
				c = f.next();
			} while (isxdigit(c));
		}
		else {
			while (c >= '0' && c <= '7') {
				str.push_back(c);
				c = f.next();
			}
		}
		f.back(c);
	}
    return{ type, str };
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
 * @berif
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
 * @berif
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

void TokenSequence::back()
{
    if (index == 0)
        error("begin of token sequence.");
	index--;
}

bool TokenSequence::next_is(const char e)
{
    if (tokens.at(index).getType() == T_KEYWORD
        && tokens.at(index).getId() == e) {
        index++;
        return true;
    }

    return false;
}

bool TokenSequence::expect(const char id)
{
    if (next_is(id))
        return true;
    errorp(tokens.at(index).getPos(), "expect : %c", id);
    return false;
}


void TokenSequence::insertFront(TokenSequence &l)
{
    for (size_t i = l.size(); i > 0; --i) {
        tokens.insert(tokens.begin(), l.at(i - 1));
    }
}
void TokenSequence::insertBack(TokenSequence &l)
{
    for (size_t i = 0; i < l.size(); ++i) {
        tokens.push_back(l.at(i));
    }
}

void TokenSequence::insert(TokenSequence &l) {
    for (size_t i = l.size(); i > 0; --i) {
        tokens.insert(tokens.begin() + index, l.at(i - 1));
    }
}