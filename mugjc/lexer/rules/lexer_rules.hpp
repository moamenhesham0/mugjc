//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_LEXER_RULES_HPP
#define MUGJC_LEXER_RULES_HPP
#include <string_view>

namespace mug::lexer::rule {
using Token = std::string;
using Terminal = std::string;
static constexpr std::string_view LAMBDA = "\\L";
enum class Symbol {
    kRegularExpression = ':',  // Token
    kRegularDefinition = '=',  // Could be used in a Token
    kKleeneStar = '*',
    kKleenePlus = '+',
    kAlternation = '|',
    kRange = '-',
    kLeftParen = '(',
    kRightParen = ')',
    kEscape = '\\',
};

enum class RuleType {
    kRegularStatement,
    kRegularExpression,
    kRegularDefinition,
    kPunctuation,
    kKeywords,
};
}  // namespace mug::lexer::rule

#endif  // MUGJC_LEXER_RULES_HPP
