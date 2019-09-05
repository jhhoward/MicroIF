#include <algorithm>
#include <vector>
#include <iostream>
#include "StringTable.h"
#include "Game.h"

using namespace std;

struct CommonToken
{
	string token;
	int occurences = 0;
	int score = 0;
};

StringTable::StringTable(const std::set<std::string>& inStringData)
{
	cout << "Compressing strings.." << endl;
	
	constexpr bool forceUpperCase = false;
	constexpr bool addNewLines = false;
	constexpr bool compressStrings = true;
	constexpr int maxColumnWidth = 25;
	
	int originalTotalStringSize = 0;
	vector<string> encodedStrings;
	set<string> wordSet;
	
	for(const std::string& originalStr : inStringData)
	{
		string str = originalStr;
		
		if(forceUpperCase)
		{
			transform(str.begin(), str.end(), str.begin(), toupper);
		}
		
		if(addNewLines)
		{
			int outX = 0;
			int wordBegin = 0;
			
			for(size_t n = 0; n < str.size(); n++)
			{
				if(str[n] == '\n')
				{
					outX = 0;
				}
				else
				{
					if(str[n] == ' ')
					{
						wordBegin = n;
					}
					outX++;
					
					if(outX >= maxColumnWidth)
					{
						str[wordBegin] = '\n';
						outX = n - wordBegin - 1;
					}
				}
			}
		}
		
		//cout << str << endl << endl;
		
		originalTotalStringSize += str.size() + 1;
		encodedStrings.push_back(str);
		
		char* tempString = new char[str.size() + 1];
		strcpy_s(tempString, str.size() + 1, str.c_str());
		
		char* pch;
		char* nextToken = nullptr;
		pch = strtok_s(tempString, " `',!.:", &nextToken);
		while(pch)
		{
			wordSet.insert(string(pch));
			pch = strtok_s(nullptr, " `',!.:", &nextToken);
		}
		
		delete[] tempString;
	}
	
	if (compressStrings)
	{
		vector<string> codedPhrases;
		constexpr int maxCodedPhrases = 128;
		constexpr int minTokenLength = 3;

		set<string> potentialPhrasesSet;
		for (const std::string& constWord : wordSet)
		{
			string word = " " + constWord + " ";

			if (word.size() >= minTokenLength)
			{
				for (size_t start = 0; start < word.size() - minTokenLength; start++)
				{
					for (size_t end = start + minTokenLength; end < word.size(); end++)
					{
						string token = word.substr(start, end - start);
						int count = 0;

						for (const std::string& str : encodedStrings)
						{
							size_t pos = 0;

							while ((pos = str.find(token, pos)) != string::npos)
							{
								count++;
								pos += token.length();
							}
						}

						int score = (token.length() - 1) * count - (token.length() + 1);

						if (score > 0)
						{
							potentialPhrasesSet.insert(token);
						}
					}
				}
			}
		}
		vector<string> potentialPhrases;
		for (const string& str : potentialPhrasesSet)
		{
			potentialPhrases.push_back(str);
		}

		while (codedPhrases.size() < maxCodedPhrases)
		{
			string bestPhrase = "";
			int bestScore = 0;

			for (size_t n = 0; n < potentialPhrases.size(); n++)
			{
				string& token = potentialPhrases[n];
				int count = 0;

				for (const std::string& str : encodedStrings)
				{
					size_t pos = 0;

					while ((pos = str.find(token, pos)) != string::npos)
					{
						count++;
						pos += token.length();
					}
				}

				for (const std::string& str : codedPhrases)
				{
					size_t pos = 0;

					while ((pos = str.find(token, pos)) != string::npos)
					{
						count++;
						pos += token.length();
					}
				}

				int score = (token.length() - 1) * count - (token.length() + 1);

				if (score <= 0)
				{
					potentialPhrases.erase(potentialPhrases.begin() + n);
					n--;
				}
				else if (score > bestScore || bestPhrase == "")
				{
					bestPhrase = token;
					bestScore = score;
				}
			}

			if (bestPhrase == "")
				break;

			unsigned char phraseCode = (unsigned char)(codedPhrases.size() + 128);

			string replacement(1, phraseCode);

			// Replace words in word set
			set<string> newWordSet;
			for (const string& wordStr : wordSet)
			{
				string str = wordStr;
				size_t pos = str.find(bestPhrase);
				while (pos != string::npos)
				{
					str.replace(pos, bestPhrase.size(), replacement);
					pos = str.find(bestPhrase, pos + replacement.size());
				}
				newWordSet.insert(str);
			}
			wordSet = newWordSet;

			// Replace in string list
			for (string& str : encodedStrings)
			{
				size_t pos = str.find(bestPhrase);
				while (pos != string::npos)
				{
					str.replace(pos, bestPhrase.size(), replacement);
					pos = str.find(bestPhrase, pos + replacement.size());
				}
			}

			// Replace in coded phrases
			for (string& str : codedPhrases)
			{
				size_t pos = str.find(bestPhrase);
				while (pos != string::npos)
				{
					str.replace(pos, bestPhrase.size(), replacement);
					pos = str.find(bestPhrase, pos + replacement.size());
				}
			}

			// Replace in potential phrase list
			for (size_t n = 0; n < potentialPhrases.size(); n++)
			{
				string& str = potentialPhrases[n];
				size_t pos = str.find(bestPhrase);
				while (pos != string::npos)
				{
					str.replace(pos, bestPhrase.size(), replacement);
					pos = str.find(bestPhrase, pos + replacement.size());
				}

				if (str.size() < 3)
				{
					potentialPhrases.erase(potentialPhrases.begin() + n);
					n--;
				}
			}

			codedPhrases.push_back(bestPhrase);
		}


		//for(string& str : encodedStrings)
		//{
		//	cout << str << endl;
		//}
		//
		//for(string& phrase : codedPhrases)
		//{
		//	cout << phrase << endl;
		//}

		for (string& phrase : codedPhrases)
		{
			for (char c : phrase)
			{
				compressedData.push_back(c);
			}
			compressedData.push_back('\0');
		}

		auto it = inStringData.begin();

		for (string& str : encodedStrings)
		{
			stringMap[*it] = (uint16_t)compressedData.size();
			++it;

			for (char c : str)
			{
				compressedData.push_back(c);
			}
			compressedData.push_back('\0');
		}

		int compressedTotalStringSize = compressedData.size();

		cout << "Number of coded phrases: " << codedPhrases.size() << endl;
		cout << "Original size: " << originalTotalStringSize << " bytes" << endl;
		cout << "Compressed size: " << compressedTotalStringSize << " bytes" << endl;
		cout << "Saved : " << (originalTotalStringSize - compressedTotalStringSize) << " bytes" << endl;
		cout << "Compression ratio: " << ((compressedTotalStringSize * 100) / originalTotalStringSize) << "%" << endl;
	}
	else
	{
		auto it = inStringData.begin();

		for (string& str : encodedStrings)
		{
			stringMap[*it] = (uint16_t)compressedData.size();
			++it;

			for (char c : str)
			{
				compressedData.push_back(c);
			}
			compressedData.push_back('\0');
		}
	}
}

uint16_t StringTable::GetIndex(const std::string& str)
{
	if(str == "")
	{
		return 0xffff;
	}
	
	auto it = stringMap.find(str);
	if(it == stringMap.end())
	{
		throw std::exception((str + " was not found in string table!").c_str());
	}
	
	return it->second;
	
	//for(uint16_t n = 0; n < stringTable.size(); n++)
	//{
	//	if(stringTable[n] == str)
	//	{
	//		return n;
	//	}
	//}
	//
	//throw std::exception((str + " was not found in string table!").c_str());
	//return 0xffff;
}

void StringTable::Write(GameDataWriter& writer)
{
	writer.Comment("Code phrases:");
	
	int index = 0;
	
	for(char c : compressedData)
	{
		for(auto it : stringMap)
		{
			if(it.second == index)
			{
				writer.Comment(it.first);
			}
		}
		
		writer.Write((uint8_t) c);
		index++;
	}
}
