#ifndef AST_HH
#define AST_HH

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ASTNode {
  public:
    virtual ~ASTNode() = default;
    virtual void print() const = 0;
};

class Expression : public ASTNode {
  public:
    virtual ~Expression() = default;
};

class Statement : public ASTNode {
  public:
    virtual ~Statement() = default;
};

class BinaryExpression : public Expression {
  public:
    BinaryExpression(std::unique_ptr<Expression> left,
                     std::unique_ptr<Expression> right)
        : left(std::move(left))
        , right(std::move(right)) {}

    void print() const override {
        std::cout << "BinaryExpression" << std::endl;
    }

  private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class PrintStatement : public Statement {
  public:
    explicit PrintStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}

    void print() const override { std::cout << "PrintStatement" << std::endl; }

  private:
    std::unique_ptr<Expression> expression;
};

#endif