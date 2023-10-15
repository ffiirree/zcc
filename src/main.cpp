#include "cpp.h"
#include "gen.h"
#include "logging.h"
#include "zvm.h"

#include <fstream>
#include <iostream>

void Help()
{
    std::cout << "Usage: zcc [ Optionss ] file...\n"
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
                 "  -A                Print all data\n"
                 "\n";
}

void print_pp(TokenSequence& ts)
{
    for (size_t i = 0; i < ts.size(); ++i) {
        std::cout << ts.next().toString() << " ";
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    std::vector<std::string> fileName;
    bool isOnlyPP = false, to_print = false, stopAsm = false, useVM = false, debug = false,
         printAll = false;
    std::string _ofn;

    // 读取所有命令参数
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // 识别命令
    for (size_t i = 0; i < args.size(); ++i) {
        if (args.at(i).at(0) == '-') {
            switch (args.at(i).at(1)) {
            case 'E': to_print = true; break;
            case 'S': stopAsm = true; break;
            case 'P': isOnlyPP = true; break;
            case 'c': break;
            case 'o': _ofn = getOnlyFileName(args.at(++i)); break;
            case 'h': Help(); return 0;
            case 'I': break;
            case 'V': useVM = true; break;
            case 'D': debug = true; break;
            case 'A': printAll = true; break;
            default: log_e("Undefined args."); break;
            }
        }
        else {
            fileName.push_back(args.at(i));
        }
    }

    // 执行命令
    for (size_t i = 1; i < fileName.size(); ++i) {
        if (_ofn.empty()) _ofn = getOnlyFileName(fileName.at(i));

        TokenSequence is, os;
        Lex lex(fileName.at(i), is);

        // 预处理
        Preprocessor pp(isOnlyPP);
        pp.expand(is, os);

        // 是否打印
        if (to_print) print_pp(os);

        // print token sequence
        if (printAll) {
            std::ofstream tsf(_ofn + ".ts", std::ios::binary);
            if (!tsf.is_open()) log_e("Open " + _ofn + ".ts failed.");
            tsf << os;
            tsf.close();
        }

        //
        if (!isOnlyPP) {
            Parser parser(os, _ofn);
            if (printAll) parser.printSymbolTable();
            VirtualMachine vm(useVM, debug);
            Generate gen(&parser, &vm);

            if (useVM) {
                vm.run();
            }
            else {
                if (!stopAsm) {
#if defined(WIN32)
                    std::string runGccSys = "gcc " + _ofn + ".s -o" + _ofn + ".exe -m32";
#elif (__linux__)
                    std::string runGccSys = "gcc " + _ofn + ".s -o" + _ofn + " -m32";
#endif
                    auto ret = system(runGccSys.c_str());
                }
            }
        }
    }

    return 0;
}