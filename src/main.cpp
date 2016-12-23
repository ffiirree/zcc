#include <iostream>
#include "zcc.h"
#include "error.h"
#include "cpp.h"
#include "zvm.h"


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
        "  -V                use virtual machine to run the program.\n"
        "  -D                Debug(virtual machine mode)\n"
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
    bool isOnlyPP = false, to_print = false, stopAsm = false, useVM = false, debug = false;
    std::string _ofn;

	// ��ȡ�����������
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}
	
	// ʶ������
	for (size_t i = 0; i < args.size(); ++i) {
		if (args.at(i).at(0) == '-') {
			switch (args.at(i).at(1)) {
            case 'E': to_print = true;  break;
            case 'S': stopAsm = true;   break;
            case 'P': isOnlyPP = true;  break;
			case 'c': break;
			case 'o': _ofn = getOnlyFileName(args.at(++i));break;
			case 'h': Help(); return 0;
			case 'I': break;
            case 'V': useVM = true; break;
            case 'D': debug = true; break;
			default:
				error("Undefined args.");
				break;
			}
		}
		else {
			fileName.push_back(args.at(i));
		}
	}

	// ִ������
	for (size_t i = 1; i < fileName.size(); ++i)
	{
        if(_ofn.empty())
            _ofn = getOnlyFileName(fileName.at(i));

        TokenSequence is, os;
        Lex lex(fileName.at(i), is);

        // Ԥ����
        Preprocessor pp(isOnlyPP);
        pp.expand(is, os);

        // �Ƿ��ӡ
        to_print ? print_pp(os) : false;

        // 
        if (!isOnlyPP) {
            Parser parser(os, _ofn);
            VirtualMachine vm(useVM, debug);
            Generate gen(&parser, &vm);

            if (useVM) {
                vm.run();
            }
            else {
                if (!stopAsm) {
                    std::string runGccSys = "gcc " + _ofn + ".s -o" + _ofn + ".exe";
                    auto ret = system(runGccSys.c_str());
                }
            }
        }
	}

	return 0;
}