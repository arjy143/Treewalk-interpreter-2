#include "Scanner.h"

const std::unordered_map<std::string, TokenType> Scanner::keywords = {
	{"and", TokenType::AND}, {"class", TokenType::CLASS}, {"else", TokenType::ELSE},
	{"false", TokenType::FALSE}, {"for", TokenType::FOR}, {"fun", TokenType::FUN},
	{"if", TokenType::IF}, {"nil", TokenType::NIL}, {"or", TokenType::OR},
	{"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
	{"this", TokenType::THIS}, {"true", TokenType::TRUE}, {"var", TokenType::VAR},
	{"while", TokenType::WHILE}
};

std::vector<Token> Scanner::ScanTokens()
{
	while (!IsAtEnd())
	{
		start = current;
		ScanToken();
	}
	tokens.emplace_back(Token(TokenType::END_OF_FILE, "", std::monostate{}, line));
	return tokens;
}

bool Scanner::IsAtEnd() const
{
	return current >= source.length();
}
char Scanner::Advance()
{
	return source[current++];
}

char Scanner::Peek() const
{
	if (IsAtEnd()) return '\0';
	return source[current];
}

char Scanner::PeekNext() const
{
	if (current + 1 >= source.length()) return '\0';
	return source[current + 1];
}

void Scanner::AddToken(TokenType type)
{
	Scanner::AddToken(type, "");
}

//void AddToken(TokenType type, std::variant<std::monostate, double, std::string> literal) {}

void Scanner::AddToken(TokenType type, const std::string literal)
	{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(type, text, literal, line);
}

void Scanner::AddToken(TokenType type, double number) {
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(type, text, number, line);
}

void Scanner::ScanToken()
{
		char c = Advance();
	switch (c)
	{
	case '(': AddToken(TokenType::LEFT_PAREN); break;
	case ')': AddToken(TokenType::RIGHT_PAREN); break;
	case '{': AddToken(TokenType::LEFT_BRACE); break;
	case '}': AddToken(TokenType::RIGHT_BRACE); break;
	case ',': AddToken(TokenType::COMMA); break;
	case '.': AddToken(TokenType::DOT); break;
	case '-': AddToken(TokenType::MINUS); break;
	case '+': AddToken(TokenType::PLUS); break;
	case ';': AddToken(TokenType::SEMICOLON); break;
	case '*': AddToken(TokenType::STAR); break;
	case '!':
		AddToken(Peek() == '=' ? TokenType::BANG_EQUAL : TokenType::BANG);
		if (Peek() == '=') Advance();
		break;
	case '=':
		AddToken(Peek() == '=' ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
		if (Peek() == '=') Advance();
		break;
	case '<':
		AddToken(Peek() == '=' ? TokenType::LESS_EQUAL : TokenType::LESS);
		if (Peek() == '=') Advance();
		break;
	case '>':
		AddToken(Peek() == '=' ? TokenType::GREATER_EQUAL : TokenType::GREATER);
		if (Peek() == '=') Advance();
		break;
	case '/':
		if (Peek() == '/')
		{
			while (Peek() != '\n' && !IsAtEnd()) Advance();
		}
		else
		{
			AddToken(TokenType::SLASH);
		}
		break;
	case ' ':
	case '\r':
	case '\t':
		break;
	case '\n':
		line++;
		break;
	case '"': String(); break;
	default:
		if (IsDigit(c))
			Number();
		else if (IsAlpha(c))
			Identifier();
		else
			; //Lox::Error(line, "Unexpected character.");
		break;
	}
}
void Scanner::String()
{
	while (Peek() != '"' && !IsAtEnd())
	{
		if (Peek() == '\n') line++;
		Advance();
	}
	if (IsAtEnd())
	{
		//Lox::Error(line, "Unterminated string.");
		return;
	}
	Advance();
	std::string value = source.substr(start + 1, current - start - 2);
	AddToken(TokenType::STRING, value);
}

void Scanner::Number()
{
	while (IsDigit(Peek())) Advance();
	if (Peek() == '.' && IsDigit(PeekNext()))
	{
		Advance();
		while (IsDigit(Peek())) Advance();
	}
	double value = std::stod(source.substr(start, current - start));
	AddToken(TokenType::NUMBER, value);
}

void Scanner::Identifier()
{
	while (IsAlphaNumeric(Peek())) Advance();
	std::string text = source.substr(start, current - start);
	auto keyword = keywords.find(text);
	if (keyword != keywords.end())
	{
		AddToken(keyword->second);
	}
	else
	{
		AddToken(TokenType::IDENTIFIER);
	}
}

bool Scanner::IsDigit(char c) const
{
	return c >= '0' && c <= '9';
}

bool Scanner::IsAlpha(char c) const
{
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

bool Scanner::IsAlphaNumeric(char c) const
{
	return IsAlpha(c) || IsDigit(c);
}
