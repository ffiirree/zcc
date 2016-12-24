#include "lex.h"
#include "type.h"
#include "error.h"

std::vector<std::string> Lex::keywords_ = { 
#define keyword(ty, name, _) std::string(name), 
    KEYWORD_MAP
#undef keyword
};

void Lex::scan(const std::string &filename, TokenSequence &ts)
{
	f_.open(filename);
    ts.setFileName(filename);

	do {
        last_ = next();
        if (last_.getType() != OP_BACKLASH && last_.getType() != T_EOF) {
            last_.setPos(f_.getPos());
            ts.push_back(last_);
        }
	} while (last_.getType() != T_EOF);
}

Token Lex::next()
{
	char c;
	while((c = f_.next()) != 0) {
		switch (c) {
        case '\n': if (last_.getType() != OP_BACKLASH) return{ T_NEWLINE, 0 };
        case '\\': return{ OP_BACKLASH, 0 };
		case '\r': break;
        case '#':  return f_.next_is('#') ? Token(OP_DS) : Token('#');
        case '+':  return f_.next_is('+') ? Token(OP_INC) : (f_.next_is('=') ? Token(OP_A_ADD) : Token('+'));
		case '*':  return f_.next_is('=') ? Token(OP_A_MUL) : Token('*');
		case '%':  return f_.next_is('=') ? Token(OP_A_MOD) : Token('%');
		case '|':  return f_.next_is('|') ? Token(OP_LOGOR) : (f_.next_is('=') ? Token(OP_A_OR) : Token('|'));
		case '&':  return f_.next_is('&') ? Token(OP_LOGAND) : (f_.next_is('=') ? Token(OP_A_AND) : Token('&'));
		case '^':  return f_.next_is('=') ? Token(OP_A_XOR) : Token('^');
		case '=':  return f_.next_is('=') ? Token(OP_EQ) : Token('=');
		case '!':  return f_.next_is('=') ? Token(OP_NE) : Token('!');
        case '>':  return f_.next_is('>') ? (f_.next_is('=') ? Token(OP_A_SAR) : Token(OP_SAR)) : (f_.next_is('=') ? Token(OP_GE) : Token('>'));
        case '<':  return f_.next_is('<') ? (f_.next_is('=') ? Token(OP_A_SAL) : Token(OP_SAL)) : (f_.next_is('=') ? Token(OP_LE) : Token('<'));
        case '.':  return f_.next_is('.') ? (f_.next_is('.') ? Token(ELLIPSIS) : false) : Token('.');
		case '"':  return read_string();
		case '\'': return read_char();

		case_a_z: case_A_Z: case '_': return read_id(c);
		case_0_9: return read_num(c);
        _CASE_6('[', ']', '{', '}', '(', ')') : case '?': case ':': case ',': case '~': case ';': return{ c };

		case '/':
			if (f_.test('/') || f_.test('*')) {
                skipComments();
                break;
			}
			return f_.next_is('=') ? Token(OP_A_DIV) : Token('/');

		case '-':
            if (f_.next_is('=')) return{ OP_A_SUB };
            if (f_.next_is('-')) return{ OP_DEC };
            if (f_.next_is('>')) return{ OP_ARROW };
            return{ '-' };
		}
    };

    return{};
}


void Lex::skipComments()
{
    char c;
    if (f_.next_is('/')) {
        while ((c = f_.next()) != '\n' && c != 0);
    }
    else if (f_.next_is('*')) {
        while ((!(c = f_.next() == '*' && f_.next_is('/'))) && c != 0);
    }
}

int Lex::isKeyword(const std::string &str)
{
	for (size_t i = 0; i < keywords_.size(); ++i) {
		if (keywords_.at(i) == str) {
			return i + K_AUTO;
		}
	}
	return T_IDENTIFIER;
}

Token Lex::read_string()
{
	std::string s;
    char c;

    while ((c = f_.next()) != '"') s.push_back(c);

    return{ T_STRING, s };
}

Token Lex::read_char()
{
	char c = f_.next();
	int r = (c == '\\') ? read_escaped_char() : c;
	if ((c = f_.next()) != '\'')
		error("unterminated char");

    return{ T_CHAR, (char)r };
}

Token Lex::read_id(char fir)
{
	int i = T_IDENTIFIER;
	int c = fir;
	std::string str;

	do {
		str.push_back(c);
		c = f_.next();
	} while (isalnum(c) || c == '_');
	f_.back(c);

    if (last_.toString() != "#") {
        if ((i = isKeyword(str)) != T_IDENTIFIER) {
            return{ i };
        }
    }

    return{ T_IDENTIFIER, str };
}

Token Lex::read_num(char fir)
{
	char c = fir;
	int type = T_INTEGER;
	int pos = 0;
	std::string str;

	if (c > '0' || (f_.peek() == '.')) {
		do {
			str.push_back(c);
			c = f_.next();
		} while (isdigit(c));

		if (c == 'e' || c == 'E') goto _e;

		if (c == '.') {
			str.push_back(c);
			c = f_.next();
			type = T_FLOAT;
			if (isdigit(c)) {
				do {
					str.push_back(c);
					c = f_.next();
				} while (isdigit(c));
			_e:
				if (c == 'e' || c == 'E') {
					str.push_back(c);
					c = f_.next();

					if (!(c == '+' || c == '-' || isdigit(c)))
						error("error number!");

					if (c == '-' || c == '+') {
						str.push_back(c);
						c = f_.next();
					}

					if (isdigit(c)) {
						while (isdigit(c)) {
							str.push_back(c);
							c = f_.next();
						}
					}
				}
			}
		}
		f_.back(c);
	}
	else {
		str.push_back(c);
		c = f_.next();
		if (c == 'x' || c == 'X') {
			do {
				str.push_back(c);
				c = f_.next();
			} while (isxdigit(c));
		}
		else {
			while (c >= '0' && c <= '7') {
				str.push_back(c);
				c = f_.next();
			}
		}
		f_.back(c);
	}
    return{ type, str };
}

int Lex::read_escaped_char() {
	int c = f_.next();

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

int Lex::read_octal_char(int c) {
	int r = c - '0';
	if (!(f_.peek() >= '0' && f_.peek() <= '7'))
		return r;
	r = (r << 3) | (f_.next() - '0');
	if (!(f_.peek() >= '0' && f_.peek() <= '7'))
		return r;
	return (r << 3) | (f_.next() - '0');
}

int Lex::read_hex_char() {
	int c = f_.next();
	if (!isxdigit(c))
		error("\\x is not followed by a hexadecimal character: %c", c);
	int r = 0;
	for (;; c = f_.next()) {
		switch (c) {
		case_0_9: r = (r << 4) | (c - '0'); continue;
		case_a_f: r = (r << 4) | (c - 'a' + 10); continue;
		case_A_F: r = (r << 4) | (c - 'A' + 10); continue;
		default: f_.back(c); return r;
		}
	}
}

void TokenSequence::back()
{
    if (index_ == 0)
        error("begin of token sequence.");
	index_--;
}

bool TokenSequence::next_is(const char e)
{
    if (peek().getType() == T_KEYWORD && peek().getId() == e) {
        index_++;
        return true;
    }

    return false;
}

bool TokenSequence::expect(const char id)
{
    if (next_is(id))
        return true;
    errorp(tokens_.at(index_).getPos(), "expect : %c", id);
    return false;
}


void TokenSequence::insertFront(TokenSequence l)
{
    for (size_t i = l.size(); i > 0; --i) {
        tokens_.insert(tokens_.begin(), l.at(i - 1));
    }
}
void TokenSequence::insertBack(TokenSequence l)
{
    for (size_t i = 0; i < l.size(); ++i) {
        tokens_.push_back(l.at(i));
    }
}

void TokenSequence::insert(TokenSequence l) 
{
    for (size_t i = l.size(); i > 0; --i) {
        tokens_.insert(tokens_.begin() + index_, l.at(i - 1));
    }
}