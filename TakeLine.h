#pragma once
#include <regex>
#include <iostream>
#include <cassert>

class TakeLine {
public:
	struct Line
	{
		enum LineType { read, print, assignment, fun, error };
		LineType type;
		std::string token;
	};
	TakeLine(std::istream& _in);

	Line getNextLine();
	bool hasMoreTokens();
	static int getCountLines();

private:
	static int countLines;
	std::istream& in;

	void removeSpaces(std::string& s);
};
