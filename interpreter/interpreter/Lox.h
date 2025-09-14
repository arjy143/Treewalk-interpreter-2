#pragma once
#include <string>

class Lox
{
public:
	void RunFile(const std::string& path);
	void RunPrompt();

private:
	void Run(const std::string& source);

};