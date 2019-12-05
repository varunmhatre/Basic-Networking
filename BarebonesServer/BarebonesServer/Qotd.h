#pragma once

#include <fstream>
#include <string>
#include <vector>

class CQotd
{
public:
	
	//Constructor
	CQotd(std::string filename);

	//Get random quote
	std::string GetRandomQuote();

private:
	std::vector<std::string> quotes;
};