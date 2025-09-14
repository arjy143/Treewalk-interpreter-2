#pragma once
#include "Expr.h"

//defines the statements of the AST

class Stmt
{
public:
	struct Visitor;
	virtual ~Stmt() = default;
	virtual void Accept(Visitor& visitor) = 0;
};

struct Stmt::Visitor
{
	virtual void VisitExpressionStmt(class ExpressionStmt& stmt) = 0;
	virtual void VisitPrintStmt(class PrintStmt& stmt) = 0;
};

class ExpressionStmt : public Stmt
{
public:
	std::unique_ptr<Expr> expression;

	ExpressionStmt(std::unique_ptr<Expr> expression)
		: expression(std::move(expression)) {}

	void Accept(Visitor& visitor) override { visitor.VisitExpressionStmt(*this); }
};

class PrintStmt : public Stmt
{
	public:
	std::unique_ptr<Expr> expression;

	PrintStmt(std::unique_ptr<Expr> expression)
		: expression(std::move(expression)) {}

	void Accept(Visitor& visitor) override { visitor.VisitPrintStmt(*this); }
};
