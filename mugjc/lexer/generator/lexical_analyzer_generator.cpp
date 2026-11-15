//
// Created by moamen on 11/4/26.
//

#include "lexical_analyzer_generator.hpp"

mug::lexer::FA::NFA mug::lexer::LexicalRulesParser::BuildRulesNFA(
    std::string rules) {
    this->parser_.set_rules(rules);

    return mug::lexer::FA::NFA()
}