#include <filesystem>
#include <format>
#include <iostream>
#include "arcv.h"

int main(int argc, char** argv) {
    if(argc < 3) {
usage:
        std::cerr << "Usage: unpac <x|c|d> <filename>" << std::endl;
        return 1;
    }

    std::string program = argv[1];

    if(argc == 3) {
        std::string input, output;

        unpac::Arcv arcv;
        if(program == "x") {
            input = argv[2];
            output = std::filesystem::path(input).parent_path() / std::filesystem::path(input).stem();

            std::cout << std::format("extract: {} -> {}\n", input, output);

            arcv.openFile(input);
            arcv.extractAll(output);

            return 0;
        } else if(program == "c") {
            input = argv[2];
            output = std::filesystem::path(argv[2]).parent_path() / std::filesystem::path(std::string(argv[2]) + ".lzs");

            std::cout << std::format("create: {} -> {}\n", input, output);

            arcv.injectDirectory(input);
            arcv.finalizeFile(output, true);

            return 0;
        } else if(program == "d") {
            input = argv[2];
            output = std::filesystem::path(input).stem().string() + ".arc";

            std::cout << std::format("decompress: {} -> {}\n", input, output);

            arcv.openFile(input);
            arcv.finalizeFile(output, false);

            return 0;
        }
    }

    // last resort lol
    goto usage;

    return 0;
}
