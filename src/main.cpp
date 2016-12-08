#include <iostream>
#include "zcc.h"
#include "error.h"
#include "PP.H"


void Help()
{
	std::cout << 
		"Usage: zcc [ Optionss ] file...\n"
		"Options:\n"
		"  -I<path>          add to include path\n"
		"  -P                only preprocessed\n"
		"  -E                print preprocessed source code\n"
		"  -S                Stop before assembly (default)\n"
		"  -c                Do not run linker (default)\n"
		"  -o filename       Output to the specified file\n"
		"  -h                help\n"
		"\n";
}

int main(int argc, char *argv[])
{
	std::vector<std::string> args;
	std::vector<std::string> fileName;

	// 读取所有命令参数
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	
	// 识别命令
	for (size_t i = 0; i < args.size(); ++i) {
		if (args.at(i).at(0) == '-') {
			switch (args.at(i).at(1)) {
			case 'E':
			case 'S':
			case 'P':
			case 'c':
			case 'o': 
			case 'h': Help();return 0;
			case 'I':
			default:
				error("Undefined args.");
				break;
			}
		}
		else {
			fileName.push_back(args.at(i));
		}
	}

	// 执行命令
	for (size_t i = 1; i < args.size(); ++i)
	{
        Lex is(args.at(i));
        Lex os;

        Preprocessor pp;
        pp.expand(is, os);

        for (size_t i = 0; i < os.size(); ++i) {
            std::cout << os.next() << " ";
        }

		//Parser parser(os);

		//parser.trans_unit();

		//Generate gen(&parser);
		//gen.run();
	}

 //   std::string _fn = getOnlyFileName(args.at(1));
	//std::string runGccSys = "gcc " + _fn + ".s";
	//auto ret = system(runGccSys.c_str());

	return 0;
}