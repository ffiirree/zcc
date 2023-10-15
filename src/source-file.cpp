#include "source-file.h"

#include "logging.h"

#include <sstream>

std::string source_file::read(const std::string& filename)
{
    std::ifstream fd(filename);

    if (!fd) return {};

    std::stringstream buffer;
    buffer << fd.rdbuf();

    return buffer.str();
}

source_file::source_file(const std::string& filename) { open(filename); }

void source_file::open(const std::string& filename)
{
    location_.file_name = filename;

    buffer_ = read(filename);
}

char source_file::next()
{
    if (index_ >= buffer_.length()) return 0;

    char ch = buffer_[index_];
    ++index_;

    if (ch == '\n') {
        location_.line++;
        location_.column = 1;
    }
    else {
        location_.column++;
    }

    return ch;
}

void source_file::back(char c)
{
    if (c == '\n') {
        location_.line--;
        location_.column = 1;
    }
    else if (c != 0l) {
        location_.column--;
    }

    --index_;
}

char source_file::peek() { return index_ >= buffer_.length() ? 0 : buffer_[index_]; }

char source_file::peek2() { return (index_ + 1) >= buffer_.length() ? 0 : buffer_[index_ + 1]; }

bool source_file::test(char c) { return peek() == c; }

bool source_file::test2(char c) { return peek2() == c; }

bool source_file::next_is(char ch)
{
    char c = next();
    if (c == ch) return true;
    back(c);
    return false;
}
