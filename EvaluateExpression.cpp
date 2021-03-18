#include "stdafx.h"
#include "EvaluateExpression.h"


EvaluateExpression::EvaluateExpression(std::string expr) :expression(expr)
{
	removeSpaces();
}

int EvaluateExpression::evaluate() {
	Stack<unsigned long int> values;
	Stack<char> ops;
	
	for (int i = 0; i < expression.length(); i++)
	{
		if (expression[i] == '(')
		{
			ops.push(expression[i]);
		}
		else if (isdigit(expression[i])) 
		{
			int val = 0;

			while (i < expression.length() && isdigit(expression[i]))
			{
				val = (val * 10) + (expression[i] - '0');
				i++;
			}
			values.push(val);
			i--;
			continue;
		}
		else if (expression[i] == ')')
		{
			while (!ops.isEmpty() && ops.top() != '(')
			{
				if (values.getSize() < 2)
				{
					std::cout << "Not enough values for the given operator at line " << TakeLine::getCountLines() << "!" << std::endl;
					assert(false);
				}
				unsigned long int val2 = values.top();
				values.pop();

				unsigned long int val1 = values.top();
				values.pop();

				char op = ops.top();
				ops.pop();

				values.push(doOperation(val1, val2, op));
			}

			//Трябва да ни е останала отварящата скоба!
			if (ops.isEmpty()) 
			{
				std::cout << "Mismatched brackets at line " << TakeLine::getCountLines() << "!" << std::endl;
				assert(false);
			}
			ops.pop();
		}
		else if (isOperator(expression[i])) 
		{
			//Докато топът на ops има по-голям или равен приоритет
			//на този на настоящия оператор, изпълняваме операторът, който
			//е на върха на ops с най-горните две стойности от values
			while (!ops.isEmpty() && (precedence(ops.top()) >= precedence(expression[i]))) 
			{
				if (values.getSize() < 2) 
				{
					std::cout << "Not enough values for the given operator at line " << TakeLine::getCountLines() << "!" << std::endl;
					assert(false);
				}
				unsigned long int val2 = values.top();
				values.pop();

				unsigned long int val1 = values.top();
				values.pop();

				unsigned long int op = ops.top();
				ops.pop();

				values.push(doOperation(val1, val2, op));
			}

			//Пушваме текущия оператор в стека ops 
			ops.push(expression[i]);
		}
		else 
		{
			std::cout << "Incorrect expression at line " << TakeLine::getCountLines() << "!" << std::endl;
			assert(false);
		}
	}

	while (!ops.isEmpty())
	{
		if (values.getSize() < 2) 
		{
			std::cout << "Not enough values for the given operator at line " << TakeLine::getCountLines() << "!" << std::endl;
			assert(false);
		}

		int val2 = values.top();
		values.pop();

		int val1 = values.top();
		values.pop();

		char op = ops.top();
		ops.pop();

		values.push(doOperation(val1, val2, op));
	}
	return values.top();
}

int EvaluateExpression::doOperation(int left, int right, char op)
{
	switch (op)
	{
	case '+':return left + right; break;
	case '-':return left - right; break;
	case '*':return left * right; break;
	case '/':
		if (right == 0)
		{
			std::cout << "Dividing by zero!" << TakeLine::getCountLines() << "!" << std::endl;
			assert(false);
		}
		return left / right; break;
	case '%':
		if (right == 0)
		{
			std::cout << "Dividing by zero at line " <<TakeLine::getCountLines() << "!" << std::endl;
			assert(false);
		}
		return left % right; break;

	default: assert(false);
	}
}

int EvaluateExpression::precedence(char op)
{
	if (op == '+' || op == '-')
		return 1;
	if (op == '*' || op == '/' || op == '%')
		return 2;

    return 0;
}

bool EvaluateExpression::isOperator(char c)
{
	return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%');
}

void EvaluateExpression::removeSpaces()
{
	expression.erase(std::remove_if(expression.begin(), expression.end(), ::isspace), expression.end());
}