//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_LEXICAL_ANALYZER_HPP
#define MUGJC_LEXICAL_ANALYZER_HPP
#include <cstddef>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../token/token.hpp"
#include "../finite_automata/dfa.hpp"

namespace mug::lexer {
// Runs a minimal DFA over source text, one token per NextToken() call.
class LexicalAnalyzer {
public:
    LexicalAnalyzer() = default;
    explicit LexicalAnalyzer(FA::DFA dfa);

    // Replaces the text being scanned and restarts from its beginning.
    void set_source(std::string source);

    // Returns the next token, or std::nullopt at the end of the input.
    // Uses maximal munch. On an unrecognised character it reports the error
    // to stderr, skips that character and keeps scanning.
    std::optional<token::Token> NextToken();

    [[nodiscard]] const std::vector<std::string>& symbol_table() const {
        return symbolTable_;
    }

private:
    size_t Intern(const std::string& lexeme);

    FA::DFA dfa_;
    std::string source_;
    size_t cursor_ = 0;
    std::vector<std::string> symbolTable_;
    std::unordered_map<std::string, size_t> symbolIndex_;
};
}  // namespace mug::lexer

#endif  // MUGJC_LEXICAL_ANALYZER_HPP
