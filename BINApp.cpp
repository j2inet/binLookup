// BINApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "resources\resource.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <map>
#include <iterator>
#include <regex>
#include <stdexcept>




std::wstring& ltrim(std::wstring& str)
{
	auto it = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(str.begin(), it);
	return str;
}

std::wstring& rtrim(std::wstring& str)
{
	auto it = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(it.base(), str.end());
	return str;
}

std::wstring& trim(std::wstring& str)
{
	return ltrim(rtrim(str));
}



int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::wcout << L"Usage: BINApp [000000]" << std::endl
			<< std::endl
			<< L"Given the first 6 digits of a credit card number, this will attempt to identify the bank." << std::endl;
		return 1;
	}

	std::string arg1(argv[1]);
	std::wstring creditCardNumber(arg1.begin(), arg1.end());

	//If someone gives more than 6 digits, just take the first 6
	if (creditCardNumber.size() > 6)
	{
		creditCardNumber = creditCardNumber.substr(0, 6);
	}
	if (creditCardNumber.size() != 6)
	{
		std::wcout << L"The first 6 digits of a credit card must be entered." << std::endl;
		return 1;
	}

	try {
		int temp;
		temp = std::stoi(creditCardNumber);
	}
	catch (std::invalid_argument)
	{
		std::wcout << L"Ithe credit card number can only be digits" << std::endl;
		return 1;
	}

	size_t resourceSize;
	void* resourceData;
	HRSRC  hRes = FindResource(NULL, MAKEINTRESOURCE(SOURCE_BIN_LIST), L"TEXT");
	HGLOBAL hData = LoadResource(NULL, hRes);
	resourceSize = SizeofResource(NULL, hRes);
	resourceData = LockResource(hData);
	std::string resourceString((char*)resourceData, resourceSize);
	std::wstring wresourceString(resourceString.begin(), resourceString.end());

	std::map<std::wstring, std::wstring> binMap;

	int start = 0;
	int minPrefix = 6;
	int maxPrefix = 6;
	int end = wresourceString.find(L"\n");
	std::wregex cardPrefixExpression(L"^[0-9]+$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	std::wregex numberRangeExpression(L"^([0-9]+)-([0-9]+)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
	while (end != -1)
	{
		std::wstring line = wresourceString.substr(start, end - start);
		auto trimmedLine = trim(line);
		bool comment = trimmedLine.empty() || trimmedLine[0] == L'#';

		int comma = line.find(L",,");
		if (!comment && (comma != -1))
		{
			//Separate the field by range expressions
			std::wstring binString = line.substr(0, comma);
			std::wstringstream binStream(trim(binString));
			std::wstring binSegment;
			std::vector<std::wstring> binSegmentList;
			while (std::getline(binStream, binSegment, L','))
			{
				binSegmentList.push_back(binSegment);
			}

			for (std::wstring binRange : binSegmentList)
			{
				std::wcout << L"Range: " << binRange << std::endl;
				
				std::wstring bank = line.substr(comma + 2);
				if (std::regex_match(binRange.c_str(), cardPrefixExpression))
				{
					std::wstring& bin = binRange;
					std::wsmatch match;
					std::regex_search(bin, match, cardPrefixExpression);
					std::wcout << L"Match: " << match[0] << std::endl;
					binMap[bin] = bank;
					auto len = bin.size();
					if (len < minPrefix)
					{
						minPrefix = len;
					}
					else if (len > maxPrefix)
					{
						maxPrefix = len;
					}
				}
				else if (std::regex_match(binRange, numberRangeExpression))
				{
					std::wsmatch match;
					std::regex_search(binRange, match, numberRangeExpression);
					std::wcout << L"Match: " << match[0] << std::endl;
					std::wstring start = match[1];
					std::wstring end = match[2];
					std::wcout << L"Start: " << start << std::endl;
					std::wcout << L"End: " << end << std::endl;


					if (start.size() != end.size())
					{
						std::wcout << L"Invalid range: " << binRange << std::endl;
						continue;
					}

					auto len = start.size();
					if (len < minPrefix)
					{
						minPrefix = len;
					}
					else if (len > maxPrefix)
					{
						maxPrefix = len;
					}

					for (int i = std::stoi(start); i <= std::stoi(end); i++)
					{
						std::wstringstream binStream;
						binStream << i;
						std::wstring bin = binStream.str();
						std::wcout << L"Range: " << bin << std::endl;
						binMap[bin] = bank;
					}
				}
				else
				{
					std::wcout << L"Invalid range: " << binRange << std::endl;
				}	
				
			}
		}
		start = end + 1;
		end = wresourceString.find(L"\n", start);

	}


	for (int numOfCharsToUse = maxPrefix; numOfCharsToUse >= minPrefix; numOfCharsToUse--)
	{
		std::wstring bin = creditCardNumber.substr(0, numOfCharsToUse);
		auto it = binMap.find(bin);
		if (it != binMap.end())
		{
			std::wcout << L"Bank: " << it->second << std::endl;
			return 0;
		}
	}

	std::wcout << L"No bank was found for that prefix" << std::endl;


}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
