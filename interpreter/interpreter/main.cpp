#include <iostream>
#include "Lox.h"

int main(int argc, char* argv[])
{
    Lox lox;

    if (argc > 2)
    {
		std::cerr << "Usage: " << argv[0] << " [optional_argument]\n";
		return 1;
    }
    else if (argc == 2)
    {
        lox.RunFile(argv[1]);
    }
    else
    {
        lox.RunPrompt();
    }
}
