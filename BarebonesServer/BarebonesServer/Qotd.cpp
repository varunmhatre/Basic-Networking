#include "Qotd.h"

//Constructor
CQotd::CQotd(std::string filename) 
{
	std::ifstream file;
	file.open(filename);
	if (file.is_open())
	{
		std::string line;
		std::string running = "";

		while (getline(file,line))
		{
			if (line != "%")
			{
				running += line + "\n";
			}
			else
			{
				quotes.push_back(running);
				running = "";
			}
		}
	}
}

//Get random quote
std::string CQotd::GetRandomQuote()
{
	int r = rand() % quotes.size();
	return quotes[r];
}