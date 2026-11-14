//
// Created by moamen on 11/4/26.
//

#include "lexical_analyzer_generator.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

#include "../finite_automata/thompson.hpp"

namespace mug::lexer {
namespace {
// Compact view of a character list: runs of 3+ become "a-z".
std::string FormatClass(const std::vector<char>& chars) {
    std::string result;
    size_t i = 0;
    while (i < chars.size()) {
        size_t j = i;
        while (j + 1 < chars.size() && chars[j + 1] == chars[j] + 1) {
            ++j;
        }
        if (j > i + 1) {
            result += std::string{chars[i], '-', chars[j]};
        } else if (j == i + 1) {
            result += std::string{chars[i], chars[j]};
        } else {
            result += chars[i];
        }
        result += ' ';
        i = j + 1;
    }
    return result;
}
}  // namespace

LexicalAnalyzerGenerator::LexicalAnalyzerGenerator(LexicalRulesParser parser) {
    while (parser.HasNext()) {
        for (rule::Rule& r : parser.NextRule()) {
            rules_.push_back(std::move(r));
        }
    }
}

FA::NFA LexicalAnalyzerGenerator::BuildRulesNFA() {
    using NamedFragment = std::pair<FA::Token, FA::NFA>;
    Definitions definitions;
    std::vector<NamedFragment> special;  // keywords + punctuation
    std::vector<NamedFragment> regular;  // regular expressions

    // Rules are handled in file order, so a regex can use any definition
    // that appears above it.
    for (const rule::Rule& r : rules_) {
        switch (r.type) {
            case rule::RuleType::kKeywords: {
                FA::NFA word = thompson::Literal(r.name[0]);
                for (size_t i = 1; i < r.name.size(); ++i) {
                    word = thompson::Concatenation(
                        std::move(word), thompson::Literal(r.name[i]));
                }
                special.emplace_back(r.name, std::move(word));
                break;
            }
            case rule::RuleType::kPunctuation:
                special.emplace_back(r.name, thompson::Literal(r.name[0]));
                break;
            case rule::RuleType::kRegularDefinition:
                definitions.insert_or_assign(
                    r.name, RegexParser(r.body, definitions).Parse());
                break;
            case rule::RuleType::kRegularExpression:
                regular.emplace_back(r.name,
                                     RegexParser(r.body, definitions).Parse());
                break;
            case rule::RuleType::kRegularStatement:
                break;
        }
    }

    // New start state with a lambda edge into every rule's fragment.
    FA::NFA nfa;
    const FA::StateId start = nfa.AddState();
    nfa.set_start_state(start);
    size_t priority = 0;
    for (std::vector<NamedFragment>* group : {&special, &regular}) {
        for (auto& [name, fragment] : *group) {
            const FA::StateId offset = nfa.Absorb(fragment);
            nfa.AddLambda(start, fragment.start_state() + offset);
            nfa.MarkAccepting(fragment.final_state() + offset, name,
                              priority++);
        }
    }
    return nfa;
}

LexicalAnalyzer LexicalAnalyzerGenerator::Generate() {
    const FA::NFA nfa = BuildRulesNFA();
    const FA::DFA dfa = FA::DFA::FromNFA(nfa);
    minimalDfa_ = dfa.Minimized();
    nfaStates_ = nfa.size();
    dfaStates_ = dfa.size();
    return LexicalAnalyzer(minimalDfa_);
}

std::string LexicalAnalyzerGenerator::TransitionTable() const {
    const FA::DFA& dfa = minimalDfa_;
    const size_t states = dfa.size();

    // Characters whose column is identical in every state share a class.
    std::vector<std::vector<FA::StateId>> columns;
    std::vector<std::vector<char>> classes;
    for (size_t c = 0; c < FA::InputRange; ++c) {
        std::vector<FA::StateId> column(states);
        bool used = false;
        for (size_t s = 0; s < states; ++s) {
            column[s] = dfa.Next(s, static_cast<FA::Terminal>(c));
            used = used || column[s] != FA::TrapState;
        }
        if (!used) {
            continue;
        }
        auto it = std::find(columns.begin(), columns.end(), column);
        if (it == columns.end()) {
            columns.push_back(std::move(column));
            classes.emplace_back();
            it = columns.end() - 1;
        }
        classes[it - columns.begin()].push_back(static_cast<char>(c));
    }

    std::ostringstream out;
    out << "NFA states (combined): " << nfaStates_
        << "\nDFA states (before minimization): " << dfaStates_
        << "\nMinimal DFA states: " << states << "\n\nCharacter classes:\n";
    for (size_t k = 0; k < classes.size(); ++k) {
        out << "  C" << k << " = { " << FormatClass(classes[k]) << "}\n";
    }

    out << "\n" << std::left << std::setw(10) << "State";
    for (size_t k = 0; k < classes.size(); ++k) {
        out << std::setw(5) << ("C" + std::to_string(k));
    }
    out << "Accepts\n";
    for (size_t s = 0; s < states; ++s) {
        out << std::setw(10)
            << (s == 0 ? "S0(start)" : "S" + std::to_string(s));
        for (size_t k = 0; k < classes.size(); ++k) {
            const FA::StateId to = columns[k][s];
            out << std::setw(5)
                << (to == FA::TrapState ? "-" : "S" + std::to_string(to));
        }
        if (const FA::Token* token = dfa.Accepts(s)) {
            out << *token;
        }
        out << "\n";
    }
    return out.str();
}
}  // namespace mug::lexer
