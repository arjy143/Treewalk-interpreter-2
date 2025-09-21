#pragma once
#include "Expr.h"
#include <vector>
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
	virtual void VisitVarStmt(class VarStmt& stmt) = 0;
	virtual void VisitBlockStmt(class BlockStmt& stmt) = 0;
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

class VarStmt : public Stmt
{
public:
	Token name;
	std::unique_ptr<Expr> initializer; //can be null

	VarStmt(const Token& name, std::unique_ptr<Expr> initializer)
		: name(name), initializer(std::move(initializer)) {}

	void Accept(Visitor& visitor) override { visitor.VisitVarStmt(*this); }
};

class BlockStmt : public Stmt
{
public:
	std::vector<std::unique_ptr<Stmt>> statements;

	BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
		: statements(std::move(statements)) {}

	void Accept(Visitor& visitor) override { visitor.VisitBlockStmt(*this); }
};