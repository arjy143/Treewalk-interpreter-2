#pragma once
#include "TokenType.h"
#include <string>
#include <variant>

using LoxValue = std::variant<std::monostate, double, std::string, bool>;

struct Token
{
	TokenType type;
	std::string lexeme;
	LoxValue lit;
	int line;

	Token(TokenType type, const std::string lexeme, LoxValue literal, int line)
		: type(type), lexeme(lexeme), lit(lit), line(line) {}
	Token(TokenType type, const std::string lexeme, double num, int line)
		: type(type), lexeme(lexeme), lit(num), line(line) {}
	Token(TokenType type, const std::string lexeme, std::string str, int line)
		: type(type), lexeme(lexeme), lit(str), line(line) {}
	Token(TokenType type, const std::string lexeme, bool bol, int line)
		: type(type), lexeme(lexeme), lit(bol), line(line) {}
	std::string ToString() const
	{
		return lexeme + " " + std::visit([](auto&& arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, std::monostate>)
				return "nil";
			else if constexpr (std::is_same_v<T, double>)
				return std::to_string(arg);
			else if constexpr (std::is_same_v<T, std::string>)
				return arg;
			else
				return "unknown";
		}, lit);
	}
};