#pragma once
#include "Token.h"
#include <string>
#include <stdexcept>

class RuntimeError : public std::runtime_error
{
public:
	Token token;
	RuntimeError(const Token& token, const std::string& message)
			: std::runtime_error(message), token(token) {}
		const Token& getToken() const { return token; }
};