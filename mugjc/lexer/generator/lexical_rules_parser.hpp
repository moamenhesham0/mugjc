//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_LEXICAL_RULES_PARSER_HPP
#define MUGJC_LEXICAL_RULES_PARSER_HPP
#include <fstream>
#include <vector>

namespace mug::lexer {
class LexicalRulesParser {
public:
    LexicalRulesParser(const std::string_view rules);

    ~LexicalRulesParser() = default;

    void set_rules(std::string_view rules);

    std::vector<std::string_view> NextRule();

private:
    std::string Token(std::string_view rule);
    mug::lexer::rule::RuleType RuleType(std::string_view rule);
    std::std::string_view rules_;
    size_t cursor_;
};
}  // namespace mug::lexer

#endif  // MUGJC_LEXICAL_RULES_PARSER_HPP
