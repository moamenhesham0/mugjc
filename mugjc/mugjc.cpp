//
// Created by moamen on 10/25/26.
//

#include <exception>
#include <iostream>
#include <string>

#include "io/io.hpp"
#include "lexer/generator/lexical_analyzer_generator.hpp"
#include "lexer/generator/lexical_rules_parser.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: mugjc <rules.txt> <source-file> [outdir]\n";
        return 1;
    }
    const std::string outdir = argc > 3 ? argv[3] : ".";

    const auto rules = mug::io::ReadFile(argv[1]);
    if (!rules) {
        std::cerr << "cannot open rules file\n";
        return 1;
    }
    auto source = mug::io::ReadFile(argv[2]);
    if (!source) {
        std::cerr << "cannot open source file\n";
        return 1;
    }

    try {
        mug::lexer::LexicalAnalyzerGenerator generator{
            mug::lexer::LexicalRulesParser(*rules)};
        mug::lexer::LexicalAnalyzer analyzer = generator.Generate();

        if (!mug::io::WriteFile(outdir + "/transition_table.txt",
                                generator.TransitionTable())) {
            std::cerr << "cannot write transition_table.txt\n";
            return 1;
        }

        analyzer.set_source(std::move(*source));
        while (auto token = analyzer.NextToken()) {
            std::cout << token->type << "\n";
        }
        std::cout << "\n--- Symbol table ---\n";
        for (size_t i = 0; i < analyzer.symbol_table().size(); ++i) {
            std::cout << i << "\t" << analyzer.symbol_table()[i] << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
