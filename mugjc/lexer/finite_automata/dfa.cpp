//
// Created by moamen on 11/12/26.
//

#include "dfa.hpp"

#include <map>
#include <set>
#include <stdexcept>
#include <utility>

namespace mug::lexer::FA {
namespace {
std::array<StateId, InputRange> TrapRow() {
    std::array<StateId, InputRange> row;
    row.fill(TrapState);
    return row;
}
}  // namespace

DFA DFA::FromNFA(const NFA& nfa) {
    if (nfa.size() == 0) {
        throw std::invalid_argument("DFA: cannot build from an empty NFA");
    }

    DFA dfa;
    std::map<std::set<StateId>, StateId> ids;
    std::vector<std::set<StateId>> subsets;

    // Returns the id of a subset, creating a new DFA state if it is new.
    auto intern = [&](std::set<StateId> subset) -> StateId {
        auto [it, inserted] =
            ids.try_emplace(std::move(subset), subsets.size());
        if (inserted) {
            subsets.push_back(it->first);
            dfa.table_.push_back(TrapRow());
        }
        return it->second;
    };

    intern(nfa.LambdaClosure({nfa.start_state()}));
    // `subsets` grows while we iterate: that is the work list.
    for (StateId i = 0; i < subsets.size(); ++i) {
        for (size_t c = 0; c < InputRange; ++c) {
            std::set<StateId> moved = nfa.Move(subsets[i], c);
            if (moved.empty()) {
                continue;
            }
            const StateId target = intern(nfa.LambdaClosure(std::move(moved)));
            dfa.table_[i][c] = target;
        }
    }

    // A DFA state accepts the rule with the lowest priority number among the
    // accepting NFA states it contains (earlier rule wins).
    for (StateId i = 0; i < subsets.size(); ++i) {
        const NFA::Acceptance* best = nullptr;
        for (StateId u : subsets[i]) {
            auto it = nfa.accepting_states().find(u);
            if (it != nfa.accepting_states().end() &&
                (best == nullptr || it->second.priority < best->priority)) {
                best = &it->second;
            }
        }
        if (best != nullptr) {
            dfa.acceptingStates_[i] = best->token;
        }
    }
    return dfa;
}

DFA DFA::Minimized() const {
    const size_t n = table_.size();
    if (n == 0) {
        return *this;
    }
    const StateId dead = n;  // extra state standing in for TrapState

    // Initial partition: group 0 = non-accepting (and dead); every token gets
    // its own group, so states accepting different tokens never merge.
    std::vector<size_t> group(n + 1, 0);
    std::unordered_map<Token, size_t> token_group;
    for (const auto& [state, token] : acceptingStates_) {
        auto [it, inserted] =
            token_group.try_emplace(token, token_group.size() + 1);
        group[state] = it->second;
    }
    size_t group_count = token_group.size() + 1;

    while (true) {
        // Signature = own group + group reached on every character.
        std::map<std::vector<size_t>, size_t> signatures;
        std::vector<size_t> next(n + 1);
        for (StateId s = 0; s <= n; ++s) {
            std::vector<size_t> signature;
            signature.reserve(InputRange + 1);
            signature.push_back(group[s]);
            for (size_t c = 0; c < InputRange; ++c) {
                const StateId t = (s == dead) ? TrapState : table_[s][c];
                signature.push_back(group[t == TrapState ? dead : t]);
            }
            auto [it, inserted] =
                signatures.try_emplace(std::move(signature), signatures.size());
            next[s] = it->second;
        }
        // Refinement only ever splits groups, so an unchanged count means the
        // partition is stable.
        const bool stable = signatures.size() == group_count;
        group = std::move(next);
        group_count = signatures.size();
        if (stable) {
            break;
        }
    }

    // Renumber: start group -> 0, dead group -> TrapState, others in order.
    std::unordered_map<size_t, StateId> remap;
    StateId next_id = 0;
    remap[group[0]] = next_id++;
    remap.try_emplace(group[dead], TrapState);
    for (StateId s = 0; s < n; ++s) {
        if (remap.try_emplace(group[s], next_id).second) {
            ++next_id;
        }
    }

    DFA result;
    result.table_.assign(next_id, TrapRow());
    for (StateId s = 0; s < n; ++s) {
        const StateId id = remap[group[s]];
        if (id == TrapState) {
            continue;  // equivalent to the dead state: drop it
        }
        for (size_t c = 0; c < InputRange; ++c) {
            const StateId t = table_[s][c];
            result.table_[id][c] =
                (t == TrapState) ? TrapState : remap[group[t]];
        }
        if (auto it = acceptingStates_.find(s); it != acceptingStates_.end()) {
            result.acceptingStates_[id] = it->second;
        }
    }
    return result;
}

StateId DFA::Next(StateId state, Terminal on) const {
    if (state >= table_.size() || on >= InputRange) {
        return TrapState;
    }
    return table_[state][on];
}

const Token* DFA::Accepts(StateId state) const {
    auto it = acceptingStates_.find(state);
    return it == acceptingStates_.end() ? nullptr : &it->second;
}
}  // namespace mug::lexer::FA
