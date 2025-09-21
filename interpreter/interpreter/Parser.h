#pragma once
#include <vector>
#include <memory>
#include "Token.h"
#include "Stmt.h"
#include <stdexcept>

class ParseError : public std::runtime_error
{
public:
	ParseError(const std::string& message) : std::runtime_error(message) {}
};

class Parser
{
public:
	Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
	std::vector<std::unique_ptr<Stmt>> Parse();


private:
	const std::vector<Token>& tokens;
	int current = 0;

	//grammar rules
	std::unique_ptr<Stmt> Statement();
	std::unique_ptr<Stmt> PrintStatement();
	std::unique_ptr<Stmt> ExpressionStatement();
	std::unique_ptr<Stmt> Declaration();
	std::unique_ptr<Stmt> VarDeclaration();

	std::unique_ptr<Expr> Expression();
	std::unique_ptr<Expr> Equality();
	std::unique_ptr<Expr> Comparison();
	std::unique_ptr<Expr> Term();
	std::unique_ptr<Expr> Factor();
	std::unique_ptr<Expr> Unary();
	std::unique_ptr<Expr> Primary();
	std::unique_ptr<Expr> Assignment();

	//helper methods
	bool Match(std::initializer_list<TokenType> types);
	bool Check(TokenType type) const;
	const Token Advance();
	bool IsAtEnd() const;
	const Token Peek() const;
	const Token Previous() const;
	const Token Consume(TokenType type, const std::string& message);
	ParseError error(const Token& token, const std::string& message);
	void Synchronise();

};