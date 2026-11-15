//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
#define MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
#include "../finite_automata/nfa.hpp"
#include "lexical_rules_parser.hpp"

class LexicalAnalyzerGenerator {
public:
    LexicalAnalyzerGenerator();
    LexicalAnalyzerGenerator(mug::lexer::LexicalRulesParser);

    mug::lexer::FA::NFA BuildRulesNFA(std::string rules);

private:
    mug::lexer::LexicalRulesParser parser_;
};

#endif  // MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
