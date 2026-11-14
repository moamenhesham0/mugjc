//
// Created by moamen on 11/4/26.
//

#include "lexical_rules_parser.hpp"

mug::lexer::LexicalRulesParser::LexicalRulesParser(const std::string_view rules)
    : rules_(rules), cursor_(0) {}

void mug::lexer::LexicalRulesParser::set_rules(std::string_view rules) {
    this->rules_ = rules;
    this->cursor_ = 0;
}

std::vector<std::string_view> mug::lexer::LexicalRulesParser::NextRule() {
    std::vector<std::string_view> parsed_rule;

    auto start = cursor_;
    auto end = rules_.find('\n', cursor_);

    if (end == std::string_view::npos) {
        return parsed_rule;
    }

    cursor_ = end + 1;

    std::string_view line = rules_.substr(start, end);
    std::string line_filtered = std::string(line).replace(' ', '');

    for (auto c : line) {
    }
}
