#pragma once
#include "Expr.h"
#include "Stmt.h"
#include <vector>
#include <memory>
#include "Environment.h"

class Interpreter : public Expr::Visitor, public Stmt::Visitor
{
public:
	Interpreter() = default;

	//interpret list of statements
	void Interpret(const std::vector<std::unique_ptr<Stmt>>& statements);

	//expr visitor methods
	void VisitBinaryExpr(BinaryExpr& expr) override;
	void VisitGroupingExpr(GroupingExpr& expr) override;
	void VisitLiteralExpr(LiteralExpr& expr) override;
	void VisitUnaryExpr(UnaryExpr& expr) override;
	void VisitVariableExpr(VariableExpr& expr) override;
	void VisitAssignExpr(AssignExpr& expr) override;
	
	//stmt visitor methods
	void VisitExpressionStmt(ExpressionStmt& stmt) override;
	void VisitPrintStmt(PrintStmt& stmt) override;
	void VisitVarStmt(VarStmt& stmt) override;
	void VisitBlockStmt(BlockStmt& stmt) override;


private:
	std::variant<std::monostate, double, std::string, bool> Evaluate(Expr& expr);
	void Execute(Stmt& stmt);
	bool IsTruthy(const std::variant<std::monostate, double, std::string, bool>& value) const;
	bool IsEqual(const std::variant<std::monostate, double, std::string, bool>& a, const std::variant<std::monostate, double, std::string, bool>& b) const;
	std::string Stringify(const std::variant<std::monostate, double, std::string, bool>& value) const;

	std::variant<std::monostate, double, std::string, bool> lastValue;
	std::shared_ptr<Environment> environment = std::make_shared<Environment>();

};