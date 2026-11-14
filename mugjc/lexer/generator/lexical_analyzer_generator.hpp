//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
#define MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
#include <cstddef>
#include <string>
#include <vector>

#include "lexical_analyzer.hpp"
#include "lexical_rules_parser.hpp"

namespace mug::lexer {
// regex rules -> combined NFA -> DFA -> minimal DFA -> LexicalAnalyzer
class LexicalAnalyzerGenerator {
public:
    LexicalAnalyzerGenerator() = default;

    // Reads every rule from the parser right away.
    explicit LexicalAnalyzerGenerator(LexicalRulesParser parser);

    // One NFA that recognises every token. Rule priority (lower wins):
    // keywords and punctuation first, then regular expressions in file order.
    mug::lexer::FA::NFA BuildRulesNFA();

    // Runs the whole pipeline and returns the finished lexer.
    LexicalAnalyzer Generate();

    // Human-readable transition table of the last Generate() call, with
    // characters that behave identically merged into classes.
    [[nodiscard]] std::string TransitionTable() const;

private:
    std::vector<rule::Rule> rules_;
    FA::DFA minimalDfa_;
    size_t nfaStates_ = 0;
    size_t dfaStates_ = 0;
};
}  // namespace mug::lexer

#endif  // MUGJC_LEXICAL_ANALYZER_GENERATOR_HPP
