#pragma once
#include "Expr.h"
#include <vector>
#include <string>
class AstPrinter : public Expr::Visitor
{
public:
	std::string Print(Expr& expr);
	void VisitBinaryExpr(BinaryExpr& expr) override;
	void VisitGroupingExpr(GroupingExpr& expr) override;
	void VisitLiteralExpr(LiteralExpr& expr) override;
	void VisitUnaryExpr(UnaryExpr& expr) override;

private:
	void Parenthesise(const std::string& name, Expr& expr);
	void Parenthesise(const std::string& name, Expr& left, Expr& right);
	void Parenthesise(const std::string& name, const std::vector<const Expr*>& expressions);

	//ostringstream?
	std::string output;
};