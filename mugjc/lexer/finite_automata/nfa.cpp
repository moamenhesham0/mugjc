//
// Created by moamen on 11/13/26.
//

#include "nfa.hpp"

#include <stdexcept>
#include <utility>

namespace mug::lexer::FA {
void NFA::CheckState(StateId state) const {
    if (state >= states_.size()) {
        throw std::out_of_range("NFA: unknown state id");
    }
}

StateId NFA::AddState() {
    states_.emplace_back();
    return states_.size() - 1;
}

void NFA::AddTransition(StateId from, Terminal on, StateId to) {
    CheckState(from);
    CheckState(to);
    if (on >= InputRange) {
        throw std::invalid_argument("NFA: only ASCII input is supported");
    }
    states_[from].edges.push_back({on, to});
}

void NFA::AddLambda(StateId from, StateId to) {
    CheckState(from);
    CheckState(to);
    states_[from].lambdas.push_back(to);
}

void NFA::MarkAccepting(StateId state, Token token, size_t priority) {
    CheckState(state);
    acceptingStates_[state] = {std::move(token), priority};
}

StateId NFA::Absorb(const NFA& other) {
    const StateId offset = states_.size();
    for (const State& s : other.states_) {
        State copy;
        for (const Edge& e : s.edges) {
            copy.edges.push_back({e.on, e.to + offset});
        }
        for (StateId l : s.lambdas) {
            copy.lambdas.push_back(l + offset);
        }
        states_.push_back(std::move(copy));
    }
    for (const auto& [id, acceptance] : other.acceptingStates_) {
        acceptingStates_[id + offset] = acceptance;
    }
    return offset;
}

std::set<StateId> NFA::LambdaClosure(std::set<StateId> states) const {
    std::vector<StateId> stack(states.begin(), states.end());
    while (!stack.empty()) {
        const StateId u = stack.back();
        stack.pop_back();
        for (StateId v : states_[u].lambdas) {
            if (states.insert(v).second) {
                stack.push_back(v);
            }
        }
    }
    return states;
}

std::set<StateId> NFA::Move(const std::set<StateId>& states,
                            Terminal on) const {
    std::set<StateId> result;
    for (StateId u : states) {
        for (const Edge& e : states_[u].edges) {
            if (e.on == on) {
                result.insert(e.to);
            }
        }
    }
    return result;
}
}  // namespace mug::lexer::FA
