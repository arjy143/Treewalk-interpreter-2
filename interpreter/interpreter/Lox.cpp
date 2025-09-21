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
	//suport multi line input if { is not closed
	std::string buffer;
	int openBraces = 0;

	while (true) {
		
		std::string lineStart = ">";
		for (int i = 1; i < openBraces+1; ++i)
		{
			lineStart = "-" + lineStart;
		}
		std::cout << lineStart;
		std::string line;
		if (!std::getline(std::cin, line)) break;
		buffer += line + "\n";
		openBraces += std::count(line.begin(), line.end(), '{');
		openBraces -= std::count(line.begin(), line.end(), '}');
		if (openBraces > 0) continue;

		Run(buffer); 
		buffer.clear();
		openBraces = 0;
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