/*#ifndef PARSING_FILE_
#define PARSING_FILE_

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

void ExtractData()
{
	std::ifstream fs;
	fs.open("webSource.txt");
	std::map<std::string, bool> m{
		{"title", 1},
		{"h1", 1},
		{"p", 1},
	};

	if (!fs.is_open())
	{
		std::cerr << "error opening file" << std::endl;
		exit(1);
	}
	else
	{
		std::string s, tag, sub = "", end;
		while (!fs.eof())
		{
			while (getline(fs, s, '>'))
			{
				std::stringstream ss(s);
				getline(ss, tag, ' ');
				tag.erase(0, 1);
				if (m.at(tag))
				{
					do{
						getline(ss, s, '<');
						sub += s;
						getline(ss, end, '>');
					} while (end != "/" + tag);
					std::cout << sub << std::endl;

				}
				else
				{
					fs.ignore(256, '>');
				}
			}
		}
	}
}

#endif PARSING_FILE_*/