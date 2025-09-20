#pragma once
#include <string>
#include "RuntimeError.h"

class Lox
{
public:
	static bool hadError;
	static bool hadRuntimeError;

	void RunFile(const std::string& path);
	void RunPrompt();

	static void Error(int line, std::string message);
	static void TrackRuntimeError(const RuntimeError& message);

private:
	void Run(const std::string& source);
	
	static void Report(int line, std::string where, std::string message);
};