#pragma once
#include <vector>
#include <string>
#include <exception>
#include <map>
#include <set>

class GameDataWriter;

class StringTable
{
public:
	StringTable(const std::set<std::string>& inStringData);
	
	uint16_t GetIndex(const std::string& str);
	
	void Write(GameDataWriter& writer);

private:	
	std::map<std::string, uint16_t> stringMap;
	std::vector<char> compressedData;
};
