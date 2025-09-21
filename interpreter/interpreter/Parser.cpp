#include "Parser.h"
#include <iostream>

std::vector<std::unique_ptr<Stmt>> Parser::Parse()
{
	std::vector<std::unique_ptr<Stmt>> statements;
	while (current < tokens.size() && tokens[current].type != TokenType::END_OF_FILE) {
		statements.push_back(Declaration());
	}
	return statements;
}

//statements

std::unique_ptr<Stmt> Parser::Declaration()
{
	try
	{
		if (Match({ TokenType::VAR })) return VarDeclaration();
		return Statement();
	}
	catch (const ParseError&)
	{
		Synchronise();
		return nullptr;
	}
}

std::unique_ptr<Stmt> Parser::Statement()
{
	try
	{
		//if (Match({ TokenType::VAR })) return VarDeclaration();
		if (Match({ TokenType::PRINT })) return PrintStatement();
		if (Match({ TokenType::LEFT_BRACE })) return BlockStatement();
		return ExpressionStatement();
	}
	catch (const ParseError&)
	{
		Synchronise();
		return nullptr;
	}
}

std::unique_ptr<Stmt> Parser::PrintStatement()
{
	auto expr = Expression();
	Consume(TokenType::SEMICOLON, "Expect ';' after expression.");
	return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::ExpressionStatement()
{
	auto expr = Expression();
	Consume(TokenType::SEMICOLON, "Expect ';' after expression.");
	return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::VarDeclaration()
{
	Token name = Consume(TokenType::IDENTIFIER, "Expect variable name.");

	std::unique_ptr<Expr> initializer = nullptr;
	if (Match({ TokenType::EQUAL })) {
		initializer = Expression();
	}

	Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
	return std::make_unique<VarStmt>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::BlockStatement()
{
	std::vector<std::unique_ptr<Stmt>> statements;

	while (!Check(TokenType::RIGHT_BRACE) && !IsAtEnd()) {
		statements.push_back(Declaration());
	}

	Consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
	return std::make_unique<BlockStmt>(std::move(statements));
}

//expressions

std::unique_ptr<Expr> Parser::Expression()
{
	return Assignment();
}

std::unique_ptr<Expr> Parser::Equality()
{
	auto expr = Comparison();

	while (Match({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {
		Token op = Previous();
		auto right = Comparison();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::Comparison()
{
	auto expr = Term();

	while (Match({ TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL })) {
		Token op = Previous();
		auto right = Term();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::Term()
{
	auto expr = Factor();

	while (Match({ TokenType::MINUS, TokenType::PLUS })) {
		Token op = Previous();
		auto right = Factor();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::Factor()
{
	auto expr = Unary();

	while (Match({ TokenType::SLASH, TokenType::STAR })) {
		Token op = Previous();
		auto right = Unary();
		expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
	}

	return expr;
}

std::unique_ptr<Expr> Parser::Unary()
{
	if (Match({ TokenType::BANG, TokenType::MINUS })) {
		Token op = Previous();
		auto right = Unary();
		return std::make_unique<UnaryExpr>(op, std::move(right));
	}
	return Primary();
}

std::unique_ptr<Expr> Parser::Primary()
{
	if (Match({ TokenType::FALSE })) return std::make_unique<LiteralExpr>(false);
	if (Match({ TokenType::TRUE })) return std::make_unique<LiteralExpr>(true);
	if (Match({ TokenType::NIL })) return std::make_unique<LiteralExpr>(std::monostate{});
	if (Match({ TokenType::NUMBER, TokenType::STRING })) {
		return std::make_unique<LiteralExpr>(Previous().lit);
	}
	if (Match({ TokenType::IDENTIFIER })) {
		return std::make_unique<VariableExpr>(Previous());
	}

	if (Match({ TokenType::LEFT_PAREN })) {
		auto expr = Expression();
		Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
		return std::make_unique<GroupingExpr>(std::move(expr));
	}

	throw error(Peek(), "Expect expression.");
}

//check for equals vs assignment, and if assignment then move into left
std::unique_ptr<Expr> Parser::Assignment()
{
	auto expr = Equality();

	if (Match({ TokenType::EQUAL })) {
		Token equals = Previous();
		auto value = Assignment();

		if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
			Token name = varExpr->name;
			return std::make_unique<AssignExpr>(name, std::move(value));
		}

		error(equals, "Invalid assignment target.");
	}

	return expr;
}
//helper methods

bool Parser::Match(std::initializer_list<TokenType> types)
{
	for (TokenType type : types) {
		if (Check(type)) {
			Advance();
			return true;
		}
	}
	return false;
}

bool Parser::Check(TokenType type) const
{
	if (IsAtEnd()) return false;
	return Peek().type == type;
}

const Token Parser::Advance()
{
	if (!IsAtEnd()) current++;
	Token a = Previous();
	return a;
}

bool Parser::IsAtEnd() const
{
	return Peek().type == TokenType::END_OF_FILE;
}

const Token Parser::Peek() const
{
	Token a = tokens[current];
	return a;
}

const Token Parser::Previous() const
{
	return tokens[current - 1];
}

const Token Parser::Consume(TokenType type, const std::string& message)
{
	if (Check(type)) return Advance();
	throw error(Peek(), message);
}

ParseError Parser::error(const Token& token, const std::string& message)
{
	//report error to some error handler
	std::cerr << "[line " << token.line << "] Error";
	if (token.type == TokenType::END_OF_FILE) 
	{
		std::cerr << " at end";
	}
	else 
	{
		std::cerr << " at '" << token.lexeme << "'";
	}
	std::cerr << ": " << message << std::endl;

	return ParseError(message);
}

void Parser::Synchronise()
{
	Advance();

	while (!IsAtEnd()) {
		if (Previous().type == TokenType::SEMICOLON) return;

		switch (Peek().type) {
		case TokenType::CLASS:
		case TokenType::FUN:
		case TokenType::VAR:
		case TokenType::FOR:
		case TokenType::IF:
		case TokenType::WHILE:
		case TokenType::PRINT:
		case TokenType::RETURN:
			return;
		default:
			break;
		}

		Advance();
	}
}