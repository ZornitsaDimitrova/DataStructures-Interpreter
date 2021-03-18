#pragma once
#include "TakeLine.h"
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <utility>
#include "EvaluateExpression.h"
#include "HashMap.h"

class Interpreter {
public:
	void evaluate();
	Interpreter(TakeLine& _input);
private:
	
	TakeLine& input;
	HashMap<unsigned long int> varMap = HashMap<unsigned long int>(20);
	HashMap<std::string> funMap = HashMap<std::string>(20);

	std::string executeExpression(int startPos, TakeLine::Line::LineType type, std::string token);

	void executeAssignment(TakeLine::Line assignment);
	void executeFunctionDefinition(TakeLine::Line fun);
	void executePrinting(TakeLine::Line toPrint);
	void executeReading(TakeLine::Line read);

	std::string getArgument(std::string& token, int i);
	std::string evaluateFunctionsInTheArgument(std::string argument);

	void replaceVariablesWithValues(std::string& expression);
	void replaceParameterWithArgument(std::string funDef, std::string argument, std::string& result);

	template <typename T>
	void defineOrRedefine(HashMap<T>& map, std::string name, T value);
	template <typename T>
	T findFunDefOrVarVal(std::string name, HashMap<T> map);

	bool isLowerCaseLetter(char letter);
	bool isUpperCaseLetter(char letter);

	std::string getWholeVarName(std::string& token, int& i);
	std::string getWholeFunName(std::string& token, int& i);
};


template<typename T>
inline void Interpreter::defineOrRedefine(HashMap<T>& map, std::string name, T value)
{
	if (map.hasKey(name))
	{
		map[name] = value;
		std::cout << name << " was REDEFINED!" << std::endl;
	}
	else
	{
		map[name] = value;
		std::cout << name << " was DEFINED!" << std::endl;
	}
}

template<typename T>
inline T Interpreter::findFunDefOrVarVal(std::string name, HashMap <T> map)
{
	T value;
	if (map.hasKey(name))
	{
		value = map[name];
	}
	else
	{
		std::cout << name << " is NOT DEFINED at line " << TakeLine::getCountLines() <<"!"<< std::endl;
		assert(false);
	}
	return value;
}