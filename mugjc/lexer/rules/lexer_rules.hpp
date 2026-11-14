//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_LEXER_RULES_HPP
#define MUGJC_LEXER_RULES_HPP
#include <string>
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
    kRegularStatement,  // unrecognised line (neither of the four below)
    kRegularExpression,
    kRegularDefinition,
    kPunctuation,
    kKeywords,
};

// One logical rule. A `{ if else }` line yields one Rule per word.
struct Rule {
    RuleType type;
    std::string name;  // token / definition name, or the keyword / character
    std::string body;  // regex text (empty for keywords and punctuation)
};
}  // namespace mug::lexer::rule

#endif  // MUGJC_LEXER_RULES_HPP
