#include "file.h"
#include "error.h"

using namespace std;
void File::create(const std::string &_filename)
{
	std::fstream in;

	in.open(_filename, ios::in | ios::binary | ios::ate);
	if (!in.is_open())
		error("internal error: file open failed!");

	int size = in.tellg();
	buffer = std::shared_ptr<char>(new char[size + 1]);
	buffer.get()[size] = 0;
	in.seekg(0, ios::beg);
	in.read(buffer.get(), size);

	ptr = buffer.get();

	in.close();
}


File::File(const std::string &_filename):
	filename(_filename), pos(1, 1)
{
	create(_filename);
}

File File::open(const std::string &_file)
{
	filename = _file;
	pos = Pos(1, 1);

	create(_file);

	return *this;
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