//
// Created by moamen on 11/12/26.
//

#ifndef MUGJC_DFA_HPP
#define MUGJC_DFA_HPP
#include <array>
#include <cstddef>
#include <unordered_map>
#include <vector>

#include "nfa.hpp"

namespace mug::lexer::FA {
class DFA {
public:
    DFA() = default;

    // Subset construction. State 0 of the result is the start state.
    static DFA FromNFA(const NFA& nfa);

    // Partition refinement. The trap state is implicit (TrapState), so the
    // result has no dead state. State 0 is still the start state.
    [[nodiscard]] DFA Minimized() const;

    // Returns TrapState when there is no transition.
    [[nodiscard]] StateId Next(StateId state, Terminal on) const;

    // The token accepted in `state`, or nullptr if it is not accepting.
    [[nodiscard]] const Token* Accepts(StateId state) const;

    [[nodiscard]] size_t size() const { return table_.size(); }

private:
    std::vector<std::array<StateId, InputRange>> table_;
    std::unordered_map<StateId, Token> acceptingStates_;
};
}  // namespace mug::lexer::FA

#endif  // MUGJC_DFA_HPP
