#include "Lox.h"
#include "Scanner.h"
#include <iostream>
#include <fstream>
#include "Parser.h"
#include "AstPrinter.h"
#include "Interpreter.h"

bool Lox::hadError = false;

void Lox::RunFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Could not open file: " << path << "\n";
		return;
	}
	double d;
	file.read(reinterpret_cast<char*>(&d), sizeof d);
	int n;
	std::string s;
	if (file >> n >> s)
		std::cout << "read back from file: " << d << ' ' << n << ' ' << s << '\n';
	
	Lox::Run(s);
	if (Lox::hadError)
	{
		Lox::Error(0, "some error");
	}
}
void Lox::RunPrompt()
{
	std::string line;
	for (;;)
	{
		std::cout << "> ";
		if (!std::getline(std::cin, line) || line == "exit")
			break;
		Lox::Run(line);
		Lox::hadError = false;
	}
}
void Lox::Run(const std::string & source)
{
	hadError = false;
	Scanner scanner(source);
	auto tokens = scanner.ScanTokens();
	Parser parser(tokens);
	auto expression = parser.Parse();

	if (hadError) return;

	Interpreter interpreter;
	interpreter.Interpret(expression);
	/*for (const auto& stmt : expression) {
		if (auto printStmt = dynamic_cast<PrintStmt*>(stmt.get())) {
			interpreter.Interpret(*printStmt->expression);
		}
		else if (auto exprStmt = dynamic_cast<ExpressionStmt*>(stmt.get())) {
			interpreter.Interpret(*exprStmt->expression);
		}
	}*/
}

void Lox::Error(int line, std::string message)
{
	Lox::Report(line, "", message);
}

void Lox::Report(int line, std::string where, std::string message)
{
	std::cerr << "[line " << line << "] Error " << where << ": " << message << "\n";
	hadError = true;
}