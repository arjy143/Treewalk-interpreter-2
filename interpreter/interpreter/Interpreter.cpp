#include "Interpreter.h"
#include <stdexcept>
#include <iostream>
#include "Environment.h"


void Interpreter::Interpret(const std::vector<std::unique_ptr<Stmt>>& statements)
{
	try
	{
		for (const auto& statement : statements)
		{
			if (!statement) continue; //skipping null statements
			Execute(*statement);
		}
	}
	catch (const RuntimeError& error)
	{
		std::cerr << "[line " << error.getToken().line << "] RuntimeError: "
			<< error.what() << "\n";
	}
}

//expr visitor methods
void Interpreter::VisitBinaryExpr(BinaryExpr& expr)
{
	auto left = Evaluate(*expr.left);
	auto right = Evaluate(*expr.right);

	switch (expr.op.type)
	{
	case TokenType::PLUS:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) + std::get<double>(right);
		}
		else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
		{
			lastValue = std::get<std::string>(left) + std::get<std::string>(right);
		}
		else
		{
			throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
		}
		break;
	case TokenType::MINUS:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) - std::get<double>(right);
		}
		else
		{
			throw RuntimeError(expr.op, "Operands must be numbers.");
		}
		break;
	case TokenType::STAR:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) * std::get<double>(right);
		}
		else
		{
			throw RuntimeError(expr.op, "Operands must be numbers.");
		}
		break;
	case TokenType::SLASH:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			if (std::get<double>(right) == 0)
			{
				throw RuntimeError(expr.op, "Division by zero.");
			}
			lastValue = std::get<double>(left) / std::get<double>(right);
		}
		else
		{
			throw RuntimeError(expr.op, "Operands must be numbers.");
		}
		break;
	case TokenType::GREATER:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) > std::get<double>(right);
		}
		else
		{
			throw std::exception("Operands must be numbers.");
		}
		break;
	case TokenType::GREATER_EQUAL:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) >= std::get<double>(right);
		}
		else
		{
			throw std::exception("Operands must be numbers.");
		}
		break;
	case TokenType::LESS:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) < std::get<double>(right);
		}
		else
		{
			throw std::exception("Operands must be numbers.");
		}
		break;
	case TokenType::LESS_EQUAL:
		if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
		{
			lastValue = std::get<double>(left) <= std::get<double>(right);
		}
		else
		{
			throw std::exception("Operands must be numbers.");
		}
		break;
	case TokenType::BANG_EQUAL:
		lastValue = !IsEqual(left, right);
		break;
	case TokenType::EQUAL_EQUAL:
		lastValue = IsEqual(left, right);
		break;
	default:
		throw RuntimeError(expr.op, "Unknown binary operator.");
	}

}

void Interpreter::VisitGroupingExpr(GroupingExpr& expr)
{
	lastValue = Evaluate(*expr.expression);
}

void Interpreter::VisitLiteralExpr(LiteralExpr& expr)
{
	lastValue = expr.value;
}

void Interpreter::VisitUnaryExpr(UnaryExpr& expr)
{
	auto right = Evaluate(*expr.right);

	switch (expr.op.type)
	{
	case TokenType::MINUS:
		if (std::holds_alternative<double>(right))
		{
			lastValue = -std::get<double>(right);
		}
		else
		{
			throw RuntimeError(expr.op, "Operand must be a number.");
		}
		break;
	case TokenType::BANG:
		lastValue = !IsTruthy(right);
		break;
	default:
		lastValue = nullptr;
		break;
	}
}

void Interpreter::VisitVariableExpr(VariableExpr& expr)
{
	lastValue = environment->Get(expr.name);
}

void Interpreter::VisitAssignExpr(AssignExpr& expr)
{
	auto value = Evaluate(*expr.value);
	environment->Assign(expr.name, value);
	lastValue = value;
}

void Interpreter::VisitLogicalExpr(LogicalExpr& expr)
{
	auto left = Evaluate(*expr.left);

	if (expr.op.type == TokenType::OR)
	{
		if (IsTruthy(left))
		{
			lastValue = left;
			return;
		}
	}
	else if (expr.op.type == TokenType::AND)
	{
		if (!IsTruthy(left))
		{
			lastValue = left;
			return;
		}
	}

	lastValue = Evaluate(*expr.right);
}

//stmt visitor methods
void Interpreter::VisitExpressionStmt(ExpressionStmt& stmt)
{
	Evaluate(*stmt.expression);
}

void Interpreter::VisitPrintStmt(PrintStmt& stmt)
{
	auto value = Evaluate(*stmt.expression);
	std::cout << Stringify(value) << "\n";
}

void Interpreter::VisitVarStmt(VarStmt& stmt) {
	std::variant<std::monostate, double, std::string, bool> value = std::monostate{};
	if (stmt.initializer) {
		value = Evaluate(*stmt.initializer);
	}
	environment->Define(stmt.name.lexeme, value);
}

void Interpreter::VisitBlockStmt(BlockStmt& stmt)
{
	//create a new environment for the block
	auto previous = environment;
	environment = std::make_shared<Environment>(previous);
	try
	{
		for (const auto& statement : stmt.statements)
		{
			if (!statement) continue; //skip null statements
			//Execute(*statement);
			statement->Accept(*this);
		}
	}
	catch (...)//handle exceptions of any type, but could change this
	{
		environment = previous;
		throw;
	}
	environment = previous;
}

void Interpreter::VisitIfStmt(IfStmt& stmt)
{
	auto condition = Evaluate(*stmt.condition);
	if (IsTruthy(condition))
	{
		Execute(*stmt.thenBranch);
	}
	else if (stmt.elseBranch)
	{
		Execute(*stmt.elseBranch);
	}
}

void Interpreter::VisitWhileStmt(WhileStmt& stmt)
{
	while (IsTruthy(Evaluate(*stmt.condition)))
	{
		Execute(*stmt.body);
	}
}

//some helper methods
std::variant<std::monostate, double, std::string, bool> Interpreter::Evaluate(Expr& expr)
{
	expr.Accept(*this);
	return lastValue;
}

void Interpreter::Execute(Stmt& stmt)
{
	stmt.Accept(*this);
}

bool Interpreter::IsTruthy(const std::variant<std::monostate, double, std::string, bool>& value) const
{
	if (std::holds_alternative<std::monostate>(value)) return false;
	if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
	return true;
}

bool Interpreter::IsEqual(const std::variant<std::monostate, double, std::string, bool>& a, const std::variant<std::monostate, double, std::string, bool>& b) const
{
	//handle equals for all variant possibilities
	if (a.index() != b.index()) return false;

	//if (std::holds_alternative<std::monostate>(a)) return true;
	if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) return true;
	if (std::holds_alternative<double>(a)) return std::get<double>(a) == std::get<double>(b);
	if (std::holds_alternative<std::string>(a)) return std::get<std::string>(a) == std::get<std::string>(b);
	if (std::holds_alternative<bool>(a)) return std::get<bool>(a) == std::get<bool>(b);
	return false;
}

std::string Interpreter::Stringify(const std::variant<std::monostate, double, std::string, bool>& value) const
{
	if (std::holds_alternative<std::monostate>(value)) return "nil";
	if (std::holds_alternative<double>(value))
	{
		auto number = std::get<double>(value);
		std::string text = std::to_string(number);
		//remove trailing .0 for whole numbers
		if (text.find('.') != std::string::npos)
		{
			text.erase(text.find_last_not_of('0') + 1, std::string::npos);
			if (text.back() == '.') text.pop_back();
		}
		return text;
	}
	if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
	if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
	return "nil";
}

