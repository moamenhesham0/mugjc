//
// Created by moamen on 11/13/26.
//

#include "lexical_analyzer.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

namespace mug::lexer {
LexicalAnalyzer::LexicalAnalyzer(FA::DFA dfa) : dfa_(std::move(dfa)) {}

void LexicalAnalyzer::set_source(std::string source) {
    source_ = std::move(source);
    cursor_ = 0;
}

size_t LexicalAnalyzer::Intern(const std::string& lexeme) {
    auto [it, inserted] = symbolIndex_.try_emplace(lexeme, symbolTable_.size());
    if (inserted) {
        symbolTable_.push_back(lexeme);
    }
    return it->second;
}

std::optional<token::Token> LexicalAnalyzer::NextToken() {
    while (true) {
        while (cursor_ < source_.size() &&
               std::isspace(static_cast<unsigned char>(source_[cursor_]))) {
            ++cursor_;
        }
        if (cursor_ >= source_.size()) {
            return std::nullopt;
        }

        // Run the DFA, remembering the longest accepted prefix.
        FA::StateId state = 0;
        const FA::Token* last = nullptr;
        size_t last_end = cursor_;
        for (size_t i = cursor_; i < source_.size(); ++i) {
            state = dfa_.Next(state, static_cast<unsigned char>(source_[i]));
            if (state == FA::TrapState) {
                break;
            }
            if (const FA::Token* accepted = dfa_.Accepts(state)) {
                last = accepted;
                last_end = i + 1;
            }
        }

        if (last == nullptr) {  // error recovery: report, skip one char
            const auto line = 1 + std::count(source_.begin(),
                                             source_.begin() + cursor_, '\n');
            std::cerr << "Lexical error at line " << line
                      << ": unexpected character '" << source_[cursor_]
                      << "'\n";
            ++cursor_;
            continue;
        }

        token::Token result;
        result.type = *last;
        result.lexeme = source_.substr(cursor_, last_end - cursor_);
        if (result.type == token::kIdentifier) {
            result.symbol_index = Intern(result.lexeme);
        }
        cursor_ = last_end;
        return result;
    }
}
}  // namespace mug::lexer
