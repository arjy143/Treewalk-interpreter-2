#include "Lox.h"
#include <iostream>
#include <fstream>

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
	std::cout << source << "\n";
}

void Lox::Error(int line, std::string message)
{
	Lox::Report(line, "", message);
}

void Lox::Report(int line, std::string where, std::string message)
{
	std::cerr << "[line " << line << "] Error " << where << ": " << message << "\n";
}