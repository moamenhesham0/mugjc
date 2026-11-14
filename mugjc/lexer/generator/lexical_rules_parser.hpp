//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_LEXICAL_RULES_PARSER_HPP
#define MUGJC_LEXICAL_RULES_PARSER_HPP
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../finite_automata/nfa.hpp"
#include "../rules/lexer_rules.hpp"

namespace mug::lexer {
// Splits a rules file into rule::Rule objects, one line at a time.
//
//   { int float if }      keywords
//   [ ; , ( ) ]           punctuation
//   digit = 0-9           regular definition (helper, not a token)
//   num : digit+          regular expression (a token)
//
// The parser only views the text: the caller must keep it alive.
class LexicalRulesParser {
public:
    explicit LexicalRulesParser(const std::string_view rules);

    ~LexicalRulesParser() = default;

    void set_rules(std::string_view rules);

    // True while there is another non-blank line to read.
    [[nodiscard]] bool HasNext() const;

    // Reads the next non-blank line. A keyword or punctuation line yields one
    // Rule per word; other lines yield exactly one. Throws
    // std::invalid_argument on a malformed line.
    std::vector<rule::Rule> NextRule();

private:
    std::string_view NextLine();
    static rule::RuleType RuleTypeOf(std::string_view line);

    std::string_view rules_;
    size_t cursor_;
};

using Definitions = std::unordered_map<std::string, FA::NFA>;

// Recursive-descent parser for one regex. It builds the NFA directly with
// Thompson fragments, from the weakest operator to the strongest:
//
//   Alternation   :=  Concatenation ('|' Concatenation)*
//   Concatenation :=  Unary*
//   Unary         :=  Atom ('*' | '+')*
//   Atom          :=  '(' Alternation ')' | '\' char | word | symbol
class RegexParser {
public:
    RegexParser(std::string_view regex, const Definitions& definitions);

    // Throws std::invalid_argument on a syntax error.
    FA::NFA Parse();

private:
    FA::NFA Alternation();
    FA::NFA Concatenation();
    FA::NFA Unary();
    FA::NFA Atom();
    FA::NFA RangeOrLiteral(char c);

    void SkipWhitespace();
    bool More();

    std::string_view regex_;
    size_t cursor_;
    const Definitions& definitions_;
};
}  // namespace mug::lexer

#endif  // MUGJC_LEXICAL_RULES_PARSER_HPP
