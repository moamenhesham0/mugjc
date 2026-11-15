//
// Created by moamen on 11/15/26.
//

#ifndef MUGJC_AST_HPP
#define MUGJC_AST_HPP
#include <memory>
#include <optional>
#include <string_view>

#include "../types.hpp"

namespace mug::lexer {

enum class ASTNodeType {
    kLiteral,
    kStar,
    kPlus,
    kConcatenation,
    kAlternate,
    kRange,
    kLambda
};

struct ASTNode {
    ASTNodeType type_;
    std::optional<mug::lexer::rule::Literal> literal_;
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
};

enum class ASTType {
    kKeyword,
    kRegularExpression,
    kPunctuation,
};
class AST {
public:
    AST() = default;
    ~AST() = default;

    AST(AST&&) noexcept = default;
    AST& operator=(AST&&) noexcept = default;

    AST(const AST&) = delete;
    AST& operator=(const AST&) = delete;

    AST(const std::string_view regex);

private:
    std::unique_ptr<ASTNode> root_;
};
}  // namespace mug::lexer

#endif  // MUGJC_AST_HPP
