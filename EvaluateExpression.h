#pragma once
#include <ctype.h>
#include <string>
#include <algorithm>
#include <stack>
#include <cassert>
#include <iostream>
#include "TakeLine.h"
#include "Stack.h"
class EvaluateExpression
{
public:
	EvaluateExpression(std::string expr);

	int evaluate();

private:
	std::string expression;

	int doOperation(int left, int right, char op);
	int precedence(char op);
	bool isOperator(char c);
	void removeSpaces();
};
