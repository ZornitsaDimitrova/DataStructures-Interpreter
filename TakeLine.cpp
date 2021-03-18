#include "stdafx.h"
#include "TakeLine.h"

TakeLine::TakeLine(std::istream& _in) :in(_in)
{}

int TakeLine::countLines = 0;

TakeLine::Line TakeLine::getNextLine()
{
	Line result{ TakeLine::Line::error,"" };
	std::string line;
	std::getline(in, line);
	removeSpaces(line);
	countLines++;
	if (std::regex_match(line, std::regex("read[a-z]+"))) {
		result.type = TakeLine::Line::read;
		line.erase(0, 4);
		result.token = line;
	}
	else if (line.compare(0, 5, "print") == 0) {
		result.type = TakeLine::Line::print;
		line.erase(0, 5);
		result.token = line;
	}
	else if (std::regex_match(line, std::regex("[A-Z]+\\[[a-z]+\\]=.*")))
	{
		result.type = TakeLine::Line::fun;
		result.token = line;
	}
	else if (std::regex_match(line, std::regex("[a-z]+=.*")))
	{
		result.type = TakeLine::Line::assignment;
		result.token = line;
	}
	return result;
}

bool TakeLine::hasMoreTokens()
{
	return bool(in);
}

int TakeLine::getCountLines()
{
	return countLines;
}

void TakeLine::removeSpaces(std::string& s)
{
	s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());

}


