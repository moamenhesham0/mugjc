//
// Created by moamen on 11/13/26.
//

#ifndef MUGJC_NFA_HPP
#define MUGJC_NFA_HPP
#include <array>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mug::lexer::FA {
using Terminal = unsigned char;
using Lambda = std::nullopt_t;
using Token = std::string;
using StateId = size_t;

static constexpr size_t TrapState = std::numeric_limits<StateId>::max();
static constexpr size_t InputRange =
    std::numeric_limits<unsigned char>::max() + 1;
class NFA {
    NFA();

public:
private:
    StateId startState_;
    std::unordered_map<StateId, Token> acceptingStates_;
    std::vector<std::array<std::vector<StateId>, InputRange>> transitions_;
    std::vector<std::vector<StateId>> lambdaTransitions_;
};
}  // namespace mug::lexer::FA

#endif  // MUGJC_NFA_HPP
