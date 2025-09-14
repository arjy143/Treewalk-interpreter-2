#pragma once
#include <string>

class Lox
{
public:
	static bool hadError;

	void RunFile(const std::string& path);
	void RunPrompt();
	static void Error(int line, std::string message);

private:
	void Run(const std::string& source);
	
	static void Report(int line, std::string where, std::string message);
};