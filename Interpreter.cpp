#include "stdafx.h"
#include "Interpreter.h"
#include <iostream>
#include <regex>
#include <string>


Interpreter::Interpreter(TakeLine& _input) :input(_input)
{}

void Interpreter::evaluate()
{
	TakeLine::Line tokenLine = input.getNextLine();
	while (input.hasMoreTokens())
	{
		if (tokenLine.type == TakeLine::Line::assignment)
		{
			executeAssignment(tokenLine);
		}
		else if (tokenLine.type == TakeLine::Line::fun)
		{
			executeFunctionDefinition(tokenLine);
		}
		else if (tokenLine.type == TakeLine::Line::print)
		{
			executePrinting(tokenLine);
		}
		else if (tokenLine.type == TakeLine::Line::read)
		{
			executeReading(tokenLine);
		}
		else
		{
			std::cout << "Syntax Error at line " <<TakeLine::getCountLines() << "!" << std::endl;
			assert(false);
		}
		tokenLine = input.getNextLine();
	}
}

std::string Interpreter::executeExpression(int startPos, TakeLine::Line::LineType type, std::string token)
{
	std::string result;

	for (int i = startPos; i < token.length(); i++)
	{
		 if (isUpperCaseLetter(token[i]))
		{
			std::string funName = getWholeFunName(token, i);
			std::string funDef = findFunDefOrVarVal(funName, funMap);
			if (token[i] != '[')
			{
				std::cout << "Missing \"[\" at line " << TakeLine::getCountLines() << std::endl;
				assert(false);
			}
			//основният аргумент, който може да съдържа произволно дълги изрази, съдържащи други функции
			std::string generalArgument = getArgument(token, i);
			i--;

			//крайният аргумент на външната функция след заместване на вътрешните функции (ако има такива) със съответните дефиниции 
			//спрямо дадения им аргумент
			std::string finalArgument = evaluateFunctionsInTheArgument(generalArgument);

			//навсякъде, където срещам параметъра го заменяме с изчисления финален аргумент
			replaceParameterWithArgument(funDef, finalArgument, result); 
		}
		//иначе слагам всички оператори.символи(букви от имена на променливи) и числа в резултата
		//ако имам невалиден символ или недефинирана променлива, то методите, които ги обработват, ще съобщят за това
		else
		{
			result.push_back(token[i]);
		}
	}
	//Ако типът не е дефиниране на функция накрая заменям всички променливи със стойностите им.
	//Ако имам дефиниране на функция не заменям имената на променливите със стойностите им, за да може
	//при предефиниране на дадена променлива нейната стойност да бъде променена и в дефиницията на функцията!
	if (type != TakeLine::Line::fun)
    {
    	replaceVariablesWithValues(result);
	}

	return result;
}

void Interpreter::replaceParameterWithArgument(std::string funDef, std::string argument, std::string& result)
{
	int i = 0;
	std::string parameter = getWholeVarName(funDef, i);
	//пропускам незначещото '@' след като сме взели параметъра
	i += 1;
	//заграждам дефиницията в скоби, за да съм сигурна, че операциите в нея ще се изпълнят самостоятелно, без да се влияят
	//от това какви оператори има около тях(*,/,%)
	std::string var;
	result.push_back('(');
	for (int j = i; j < funDef.length(); j++)
	{
		//ако съм стигнала малка буква знам, че следва име на променлива/параметър
		if (isLowerCaseLetter(funDef[j]))
		{
			//функцията мести j до момента, в който стига символ различен от малка буква
			var = getWholeVarName(funDef, j);
			//връщам се на последната малка буква, с цел на следващата итерация да не пропусна символа до нея
			j--;
		}
		//ако стигнатата променлива съвпада с параметъра => заменям с аргумента
		if (parameter.compare(var) == 0)
		{
			//аргумента също заграждам в скоби, защото може да не е просто число, а израз
			result.push_back('(');
			result.append(argument);
			result.push_back(')');
		}
		//ако размера на var е >0 => стигнала съм променлива, която не е параметъра => просто я добавям в резултата
		else if (var.length() > 0)
		{
			result.append(var);
		}
		//иначе просто добавям следващия символ
		else
		{
			result.push_back(funDef[j]);
		}
		//занулявам променливата var, за да може отново с нея да търся останалите променливи
		var = "";
	}
	result.push_back(')');
}

//проверки за малка буква и главна буква
bool Interpreter::isLowerCaseLetter(char letter)
{
	return letter >= 'a' && letter <= 'z';
}

bool Interpreter::isUpperCaseLetter(char letter)
{
	return letter >= 'A' && letter <= 'Z';
}

//функции, които взимат целите имена на променливи и функции
std::string Interpreter::getWholeVarName(std::string& token, int& i)
{
	std::string varName;
	while (isLowerCaseLetter(token[i]))
	{
		varName.push_back(token[i]);
		i++;
	}
	return varName;
}

std::string Interpreter::getWholeFunName(std::string& token, int& i)
{
	std::string funName;
	while (isUpperCaseLetter(token[i]))
	{
		funName.push_back(token[i]);
		i++;
	}
	return funName;
}

void Interpreter::executeAssignment(TakeLine::Line assignment)
{
	std::string var;
	int i = 0;
	var = getWholeVarName(assignment.token, i);
	//пропускам равното
	i += 1;
	//проверка за наличие на expression след равното
	if (assignment.token.size() < i + 1)
	{
		std::cout << "There is no value for the variable " << var << " at line " << TakeLine::getCountLines() << "!" << std::endl;
		assert(false);
	}
	
	std::string expr = executeExpression(i, assignment.type, assignment.token);
	EvaluateExpression final(expr);
	unsigned long int expressionResult = final.evaluate();
	defineOrRedefine(varMap, var, expressionResult);
}

void Interpreter::executeFunctionDefinition(TakeLine::Line fun)
{
	int i = 0;
	
	std::string name = getWholeFunName(fun.token, i);
	//пропускам скобата
	i++;
	std::string parameter = getWholeVarName(fun.token, i);
	
	//пропускам скобата и равното, за да стигна началото на дефиницията
	i += 2;
	//проверка за наличие на expression след равното
	if (fun.token.size() < i + 1)
	{
		std::cout << "There is no definition to the function " << name << " at line " << TakeLine::getCountLines() << "!" << std::endl;
		assert(false);
	}
	//слагам незначещ символ в края на параметъра, за да знам къде свършва името му
	parameter.push_back('@');
	std::string definition = executeExpression(i, fun.type, fun.token);
	//добавям параметъра на функцията в началото на дефиницията й.
	definition.insert(0, parameter);
	//ако имам вече дефинирана функция с това име, променям дефиницията й, ако не-създавам нова функция
	defineOrRedefine(funMap, name, definition);
}

void Interpreter::executePrinting(TakeLine::Line toPrint)
{
	std::string print = executeExpression(0, toPrint.type, toPrint.token);
	EvaluateExpression final(print);
	int expressionResult = final.evaluate();

	std::cout << expressionResult << std::endl;
}

void Interpreter::executeReading(TakeLine::Line read)
{
	int i = 0;
	std::string var = getWholeVarName(read.token, i);
	unsigned long int value;
	std::cout << "Enter value(positive integer) for variable " << var << std::endl;
	std::cin >> value;
	if ((int)value < 0)
	{
		std::cout << "Values can be only POSITIVE integers!" << std::endl;
		assert(false);
	}
	defineOrRedefine(varMap, var, value);
}

std::string Interpreter::evaluateFunctionsInTheArgument(std::string argument)
{
	std::string result;
	int startInserting;
	for (int i = 0; i < argument.length(); i++)
	{
		if (isUpperCaseLetter(argument[i]))
		{
			startInserting = i;
			std::string funName = getWholeFunName(argument, i);
			std::string innerFunDef = findFunDefOrVarVal(funName, funMap);
			//трия името на функцията, защото на негово място ще сложа дефиницията й, в която преди това заменям параметъра със съответния аргумент
			argument.erase(startInserting, funName.length());
			//getArgument маха отварящата и затварящата скоба, в които е аргументът на конкретната вътрешна функция
			std::string innerArgument = getArgument(argument, startInserting);
			replaceParameterWithArgument(innerFunDef, innerArgument, result);
			argument.insert(startInserting, result);
			result = "";
			//започвам отново от началото на стринга, защото може да има много вложени функции в аргумента, които трябва последователно да се обработят
			i = 0;
		}
	}
	return argument;
}

void Interpreter::replaceVariablesWithValues(std::string& expression)
{
	for (int i = 0; i < expression.length(); i++)
	{
		if (isLowerCaseLetter(expression[i]))
		{
			int startingVarName = i;
			std::string var = getWholeVarName(expression, i);
			//намалям итератора, т.к. след като съм стигнала края на името на променливата итератора е вече на следващия символ
			//и при следващата итерация този символ ще бъде пропуснат
			i--;

			std::string value = std::to_string(findFunDefOrVarVal(var, varMap));
			expression.erase(startingVarName, var.length());
			expression.insert(startingVarName, value);

			//i = startingVarName;
			//Горният вариант също е валиден но при по дълги числа ще се правят излишни итерации, т.к. ако стойността е по-дълго число
			//то итератора ще бъде на първата цифра от него и ненужно ще провери и всички останали.
			//Т.к. изтривам името на ппроменливата и на негово място слагам стойността й, 
			//връщам итератора назад спрямо разликата между дължините на името на променливата и нейната стойност.
			int moveBackIterator = var.length() - value.length();
			i -= moveBackIterator;
		}
	}
}

std::string Interpreter::getArgument(std::string& token, int i)
{
	int count = 0;
	int j;
	for (j = i; j < token.length(); j++) {
		if (token[j] == '[') {
			count++;
		}
		else if (token[j] == ']') {
			count--;
		}

		if (count == 0) {
			break;
		}
	}
	//взимам целия аргумент с всички използвани функции в него(ако има такива)
	if (count != 0) {
		std::cout << "Mismatched brackets at line " <<TakeLine::getCountLines() << "!" << std::endl;
		assert(false);
	}

	std::string argument;

	//махам първата отваряща скоба
	if (token[i] != '[') {
		std::cout << "Mismatched brackets at line " << TakeLine::getCountLines() << "!" << std::endl;
		assert(false);
	}
	token.erase(i, 1);
	while (j - i) {
		argument.push_back(token[i]);
		token.erase(i, 1);
		j--;
	}
	//махам последната затваряща скоба
	if (argument.back() != ']') {
		std::cout << "Mismatched brackets at line " << TakeLine::getCountLines() << "!" << std::endl;
		assert(false);
	}
	argument.pop_back();

	return argument;
}
