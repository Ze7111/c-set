#ifndef __TREE_H__
#define __TREE_H__

#include <any>
#include <future>
#include <map>
#include <stdexcept>
#include <system_error>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

#include "../../pkgs/base/include.hh"
#include "../template/token.hh"

namespace vm {
class TreeWalker;
}  // namespace vm

/*
OPTIONAL
ALWAYS
ASSIGN
SAFE
*/
#define assignment_op                                             \
    lexer::token_type::OPTIONAL : case lexer::token_type::ALWAYS: \
    case lexer::token_type::ASSIGN:                               \
    case lexer::token_type::SAFE

class AstNode {};

class Tree {
  private:
    friend class vm::TreeWalker;
    vm::TreeWalker *parent{};
    std::vector<lexer::token> base_tokens;
    std::vector<AstNode> ast;

  public:
    Tree() { throw std::runtime_error("blank tree constructor called"); }
    Tree(std::vector<lexer::token> tokens, vm::TreeWalker *parent)
        : base_tokens(std::move(tokens))
        , parent(parent) {};
    Tree(const Tree &other) = default;
    Tree(Tree &&other) noexcept = default;
    Tree &operator=(const Tree &other) = delete;
    Tree &operator=(Tree &&other) = delete;
    ~Tree() = default;

    auto get_eol(u64 current) -> u64 {
        u64 end = 0;
        for (u64 i = current; i < base_tokens.size(); ++i) {
            if (base_tokens[i].type == lexer::token_type::NEWLINE) {
                return ++end;
            }
            ++end;
        }
        return 0;
    }

    auto get_only_line(u64 &current) -> std::vector<lexer::token> {
        const u64 end = get_eol(current);
        std::vector<lexer::token> output = {base_tokens.data() + current, base_tokens.data() + end};
        current = end;
        return output;
    }

    auto generate() -> void {
        for (u64 i = 0; i < base_tokens.size(); ++i) {
            if (base_tokens[i].type == lexer::token_type::VARTYPE) {
                switch (base_tokens[1 + i].type) {
                case assignment_op:
                    std::print(colors::fg16::red, "---------------------- NODE ----------------------", colors::reset);
                    Assignment::generate_node(get_only_line(i));
                default:
                    break;
                }
            }  // if is a pure assignment
        }
    }

    class Assignment : AstNode {
      private:
        auto append_memory() {};
        friend class Tree;

      public:
        Assignment(lexer::token var_name, std::vector<lexer::token> type,
                   std::vector<lexer::token> assign);
        Assignment() = default;
        Assignment(const Assignment &other) = default;
        Assignment(Assignment &&other) noexcept = default;
        Assignment &operator=(const Assignment &other) = delete;
        Assignment &operator=(Assignment &&other) = delete;
        ~Assignment() = default;

        static auto generate_node(std::vector<lexer::token> const &line_tokens)
            -> void {
                for (lexer::token tok : line_tokens) {
                    std::print(lexer::token_to_string(tok));
                }
            }
    };
};

#endif  // __TREE_H__