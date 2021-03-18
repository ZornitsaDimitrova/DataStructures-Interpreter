// Interpreter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "EvaluateExpression.h"
#include "TakeLine.h"
#include "Interpreter.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>  

int main()
{
	std::cout << "Press ENTER if you want me to execute your EXPR program or any other key if you want to exit! :)" << std::endl;

	if (std::cin.get() == '\n')
	{
		std::filebuf program;
		if (program.open("EXPRprogram.txt", std::ios::in))
		{
			std::istream programStream(&program);
			TakeLine take(programStream);
			Interpreter interpret(take);
			interpret.evaluate();
			std::cout << std::endl;
			program.close();
		}
		else
		{
			std::cout << "Can't open the file!" << std::endl;
			assert(false);
		}
	}
	else
	{
		std::cout << "Bye! :)" << std::endl;
		return 0;
	}

	return 0;
}


