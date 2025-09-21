#pragma once
#include <memory>
#include "Token.h"

//expressions for the AST. base class for all nodes, then derived classes for each type of expression.
//every node represented as unique_ptr.

class Expr
{
public:
	struct Visitor;
	virtual ~Expr() = default;
	virtual void Accept(Visitor& visitor) = 0;
};

struct Expr::Visitor
{
	virtual void VisitBinaryExpr(class BinaryExpr& expr) = 0;
	virtual void VisitGroupingExpr(class GroupingExpr& expr) = 0;
	virtual void VisitLiteralExpr(class LiteralExpr& expr) = 0;
	virtual void VisitUnaryExpr(class UnaryExpr& expr) = 0;
	virtual void VisitVariableExpr(class VariableExpr& expr) = 0;
	virtual void VisitAssignExpr(class AssignExpr& expr) = 0;
	virtual void VisitLogicalExpr(class LogicalExpr& expr) = 0;
};

class BinaryExpr : public Expr
{
public:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(op), right(std::move(right)) {}

	void Accept(Visitor& visitor) override { visitor.VisitBinaryExpr(*this); }
};

class GroupingExpr : public Expr
{
public:
	std::unique_ptr<Expr> expression;

	GroupingExpr(std::unique_ptr<Expr> expression)
		: expression(std::move(expression)) {}

	void Accept(Visitor& visitor) override { visitor.VisitGroupingExpr(*this); }
};

class LiteralExpr : public Expr {
public:
	std::variant<std::monostate, double, std::string, bool> value;

	explicit LiteralExpr(std::variant<std::monostate, double, std::string, bool> value)
		: value(std::move(value)) {}

	void Accept(Visitor& visitor) override {
		visitor.VisitLiteralExpr(*this);
	}
};

class UnaryExpr : public Expr {
public:
	Token op;
	std::unique_ptr<Expr> right;

	UnaryExpr(Token op, std::unique_ptr<Expr> right)
		: op(op), right(std::move(right)) {}

	void Accept(Visitor& visitor) override {
		visitor.VisitUnaryExpr(*this);
	}
};

class VariableExpr : public Expr
{
public:
	Token name;
	VariableExpr(Token name) : name(name) {};
	void Accept(Visitor& visitor) override { visitor.VisitVariableExpr(*this); }
};

class AssignExpr : public Expr
{
public:
	Token name;
	std::unique_ptr<Expr> value;
	AssignExpr(Token name, std::unique_ptr<Expr> value) : name(name), value(std::move(value)) {};
	void Accept(Visitor& visitor) override { visitor.VisitAssignExpr(*this); };
};

class LogicalExpr : public Expr
{
public:
	std::unique_ptr<Expr> left;
	Token op;
	std::unique_ptr<Expr> right;

	LogicalExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
		: left(std::move(left)), op(op), right(std::move(right)) {}

	void Accept(Visitor& visitor) override { visitor.VisitLogicalExpr(*this); }
};