#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "./Generation/Generation.hpp"

int main(int argc, char* argv[]) 
{
    if (argc != 2) 
    {
        std::cerr << "Missing an argument (.gs file path missing)";
        return EXIT_FAILURE;
    }

    std::cout << argv[1] << std::endl;

    std::string contents; 
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    std::cout << contents << std::endl;

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(std::move(tokens));

    std::optional<NodeProg> prog = parser.parse_prog();
    if (!prog.has_value()) {
        std::cerr << "Invalid program" << std::endl;
        exit(EXIT_FAILURE);
    }


    Generator gen(prog.value());
    std::string asembly = gen.gen_prog();
    std::cout << asembly << std::endl;

    {
        std::fstream file("out.asm" ,std::ios::out);
        file << asembly;
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}