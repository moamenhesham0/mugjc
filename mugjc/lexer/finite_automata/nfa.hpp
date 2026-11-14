//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_NFA_HPP
#define MUGJC_NFA_HPP
#include <cstddef>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace mug::lexer::FA {
using Terminal = unsigned char;
using Token = std::string;
using StateId = size_t;

static constexpr StateId TrapState = std::numeric_limits<StateId>::max();
static constexpr size_t InputRange = 128;  // ASCII

class NFA {
public:
    struct Edge {
        Terminal on;
        StateId to;
    };
    struct Acceptance {
        Token token;
        size_t priority;  // lower wins when several rules accept together
    };

    NFA() = default;

    StateId AddState();
    void AddTransition(StateId from, Terminal on, StateId to);
    void AddLambda(StateId from, StateId to);
    void MarkAccepting(StateId state, Token token, size_t priority);

    // Copies every state of `other` into this NFA and returns the offset that
    // must be added to `other`'s state ids to get their new ids.
    StateId Absorb(const NFA& other);

    [[nodiscard]] std::set<StateId> LambdaClosure(
        std::set<StateId> states) const;
    [[nodiscard]] std::set<StateId> Move(const std::set<StateId>& states,
                                         Terminal on) const;

    [[nodiscard]] size_t size() const { return states_.size(); }
    [[nodiscard]] StateId start_state() const { return start_; }
    [[nodiscard]] StateId final_state() const { return final_; }
    void set_start_state(StateId state) { start_ = state; }
    void set_final_state(StateId state) { final_ = state; }
    [[nodiscard]] const std::unordered_map<StateId, Acceptance>&
    accepting_states() const {
        return acceptingStates_;
    }

private:
    struct State {
        std::vector<Edge> edges;
        std::vector<StateId> lambdas;
    };

    void CheckState(StateId state) const;

    std::vector<State> states_;
    std::unordered_map<StateId, Acceptance> acceptingStates_;
    StateId start_ = 0;
    StateId final_ = 0;
};
}  // namespace mug::lexer::FA

#endif  // MUGJC_NFA_HPP
