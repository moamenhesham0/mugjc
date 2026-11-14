//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
#define MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
#include "lexical_rules_parser.hpp"

class LexicalAnalyzerGenerator {
public:
    LexicalAnalyzerGenerator() = default;
    LexicalAnalyzerGenerator(mug::lexer::LexicalRulesParser);

    mug::lexer::FA::NFA BuildRulesNFA();

private:
};

#endif  // MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
