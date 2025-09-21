#include "Lox.h"
#include "Scanner.h"
#include <iostream>
#include <fstream>
#include "Parser.h"
#include "AstPrinter.h"

bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;

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
	if (Lox::hadRuntimeError)
	{
		Lox::Error(0, "some runtime error");
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
		Lox::hadRuntimeError = false;
	}
}
void Lox::Run(const std::string & source)
{
	hadError = false;
	hadRuntimeError = false;
	Scanner scanner(source);
	auto tokens = scanner.ScanTokens();
	//for (const auto& token : tokens) {
	//	std::cout << "Token: '" << token.lexeme
	//		<< "' Type: " << static_cast<int>(token.type) << std::endl;
	//}
	Parser parser(tokens);
	auto expression = parser.Parse();

	if (hadError) return;

	Lox::interpreter.Interpret(expression);

}

void Lox::Error(int line, std::string message)
{
	Lox::Report(line, "", message);
}

void Lox::TrackRuntimeError(const RuntimeError& message)
{
	std::cerr << "[line " << message.getToken().line << "] RuntimeError: "
		<< message.what() << "\n";
	hadRuntimeError = true;
}


void Lox::Report(int line, std::string where, std::string message)
{
	std::cerr << "[line " << line << "] Error " << where << ": " << message << "\n";
	hadError = true;
}