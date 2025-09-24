#pragma once
#include <memory>
#include <string>
#include "Token.h"
#include <unordered_map>
#include "RuntimeError.h"


class Environment
{
	public:
	Environment() = default;
	//Environment(std::shared_ptr<Environment> parent = nullptr) : enclosing(parent) {}
    explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}
	

	void Define(const std::string& name, const LoxValue& value)
	{
				values[name] = value;
	
	}

	void Assign(const Token& name, const LoxValue& value)
	{
		auto iter = values.find(name.lexeme);
		if (iter != values.end())
		{
			values[name.lexeme] = value;
			return;
		}
		if (enclosing != nullptr)
		{
			enclosing->Assign(name, value);
			return;
		}

		throw RuntimeError(name, "undefined variable '" + name.lexeme + "'.");
	}

	LoxValue Get(const Token& name)
	{
		auto iter = values.find(name.lexeme);
		if (iter != values.end())
		{
			return iter->second;
		}
		if (enclosing != nullptr)
		{
			return enclosing->Get(name);
		}

		throw RuntimeError(name, "undefined variable '" + name.lexeme + "'.");
	}

private:
	//keep a map of all variables and their values
	std::unordered_map<std::string, LoxValue> values;
	std::shared_ptr<Environment> enclosing;
};