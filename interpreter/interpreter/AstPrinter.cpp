#include "AstPrinter.h"

std::string AstPrinter::Print(Expr& expr)
{
	output.clear();
	expr.Accept(*this);
	return output.c_str();
}

//visitor implementations

void AstPrinter::VisitBinaryExpr(BinaryExpr& expr)
{
	Parenthesise(expr.op.lexeme, *expr.left, *expr.right);
}

void AstPrinter::VisitGroupingExpr(GroupingExpr& expr)
{
	Parenthesise("group", *expr.expression);
}

void AstPrinter::VisitLiteralExpr(LiteralExpr& expr)
{
	if (std::holds_alternative<std::monostate>(expr.value)) {
		output += "nil";
	}
	else if (std::holds_alternative<double>(expr.value)) {
		output += std::to_string(std::get<double>(expr.value));
	}
	else if (std::holds_alternative<std::string>(expr.value)) {
		output += std::get<std::string>(expr.value);
	}
	else if (std::holds_alternative<bool>(expr.value)) {
		output += std::get<bool>(expr.value) ? "true" : "false";
	}
}

void AstPrinter::VisitUnaryExpr(UnaryExpr& expr)
{
	Parenthesise(expr.op.lexeme, *expr.right);
}

//helper methods

void AstPrinter::Parenthesise(const std::string& name, Expr& expr)
{
	output += "(" + name + " ";
	expr.Accept(*this);
	output += ")";
}
void AstPrinter::Parenthesise(const std::string& name, Expr& left, Expr& right)
{
	output += "(" + name + " ";
	left.Accept(*this);
	output += " ";
	right.Accept(*this);
	output += ")";
}

void AstPrinter::Parenthesise(const std::string& name, const std::vector<const Expr*>& expressions)
{
	output += "(" + name;
	for (const auto& expr : expressions) {
		output += " ";
		const_cast<Expr*>(expr)->Accept(*this);
	}
	output += ")";
}