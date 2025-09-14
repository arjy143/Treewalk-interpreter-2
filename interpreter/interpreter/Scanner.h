#pragma once
#include <string>
#include <vector>
#include "Token.h"
#include <unordered_map>

class Scanner
{
public:
	Scanner(const std::string& source) : source(source) {}
	std::vector<Token> ScanTokens();

private:
	const std::string source;
	std::vector<Token> tokens;
	int start = 0;
	int current = 0;
	int line = 1;

	static const std::unordered_map<std::string, TokenType> keywords;
	bool IsAtEnd() const;
	char Advance();
	void AddToken(TokenType type);
	//void AddToken(TokenType type, std::variant<std::monostate, double, std::string> literal);
	void AddToken(TokenType type, const std::string literal);
	void AddToken(TokenType type, double number);

	char Peek() const;
	char PeekNext() const;
	void ScanToken();
	void String();
	void Number();
	void Identifier();
	bool IsDigit(char c) const;
	bool IsAlpha(char c) const;
	bool IsAlphaNumeric(char c) const;
};