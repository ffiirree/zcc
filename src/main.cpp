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

void print_pp(TokenSequence &ts)
{
    for (size_t i = 0; i < ts.size(); ++i) {
        std::cout << ts.next() << " ";
    }
}

int main(int argc, char *argv[])
{
	std::vector<std::string> args;
	std::vector<std::string> fileName;
    bool isOnlyPP = false, to_print = false, stopAsm = false;

	// 读取所有命令参数
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	
	// 识别命令
	for (size_t i = 0; i < args.size(); ++i) {
		if (args.at(i).at(0) == '-') {
			switch (args.at(i).at(1)) {
            case 'E': to_print = true;  break;
            case 'S': stopAsm = true;   break;
            case 'P': isOnlyPP = true;  break;
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
	for (size_t i = 1; i < fileName.size(); ++i)
	{
        std::string _ofn = getOnlyFileName(fileName.at(i));
        Lex is(fileName.at(i));
        TokenSequence os;

        // 预处理
        Preprocessor pp(isOnlyPP);
        pp.expand(is, os);

        // 是否打印
        to_print ? print_pp(os) : false;

        // 
        if (!isOnlyPP) {
            Parser parser(os, _ofn);
            Generate gen(&parser);

            if (!stopAsm) {
                std::string runGccSys = "gcc " + _ofn +".s";
                auto ret = system(runGccSys.c_str());
            }
        }
	}

	return 0;
}