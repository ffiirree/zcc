#include "file.h"
#include "error.h"

using namespace std;

File::File(const std::string &filename):
	pos(1, 1)
{
	in.open(filename, ios::in | ios::binary | ios::ate);
	if (!in.is_open())
		error("internal error: file open failed!");

	int size = in.tellg();
	buffer = new char[size + 1];
	buffer[size] = 0;
	in.seekg(0, ios::beg);
	in.read(buffer, size);

	ptr = buffer;

	in.close();
}

File::~File()
{
	if (buffer != nullptr) {
		delete[]buffer;
		buffer = nullptr;
	}
}

char File::next()
{
	char c = *ptr;
	++ptr;

	if (c == '\n') {
		pos.line++;
		pos.cols = 1;
	}
	else if(c != 0) {
		pos.cols++;
	}

	return c;
}


void File::back(char c)
{
	if (c == '\n') {
		pos.line--;
		pos.cols = 1;
	}
	else if (c != 0l) {
		pos.cols--;
	}
	--ptr;
}

char File::peek()
{
	char c = next();
	back(c);
	return c;
}