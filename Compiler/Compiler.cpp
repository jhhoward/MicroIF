#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <exception>
#include <map>
#include <set>
#include <list>
#include <vector>
#include "Parser.h"
#include "Game.h"
#include "StringTable.h"

using namespace std;

class HumanReadableGameDataWriter : public GameDataWriter
{
public:
	HumanReadableGameDataWriter(ostream& inStream) : stream(inStream) {}
	
	virtual void Write(uint8_t value) override
	{
		stream << "0x" << setfill('0') << setw(2) << hex << (int)(value) << ",";
		sizeBytes ++;
		numColumns++;
		
		if(numColumns == 40)
		{
			stream << endl;
			numColumns = 0;
		}
	}
	
	virtual void Write(uint8_t value, const string& comment) override
	{
		stream << "0x" << setfill('0') << setw(2) << hex << (int)(value);
		stream << ",				// " << sanitizeComment(comment) << endl;
		sizeBytes ++;
	}
	virtual void Write(uint8_t value, uint8_t param, const std::string& comment) override
	{
		stream << "0x" << setfill('0') << setw(2) << hex << (int)value << ",";
		stream << "0x" << setfill('0') << setw(2) << hex << (int)param;
		stream << ",			// " << sanitizeComment(comment) << endl;
		sizeBytes += 2;
	}
	virtual void Write(uint8_t value, uint16_t param, const std::string& comment) override
	{
		int lower = param & 0xff;
		int upper = (param >> 8) & 0xff;
		
		stream << "0x" << setfill('0') << setw(2) << hex << (int)value << ",";
		stream << "0x" << setfill('0') << setw(2) << hex << lower << ",";
		stream << "0x" << setfill('0') << setw(2) << hex << upper;
		stream << ",		// " << sanitizeComment(comment) << endl;
		sizeBytes += 3;
	}
	virtual void Write(uint16_t value, const string& comment) override
	{
		int lower = value & 0xff;
		int upper = (value >> 8) & 0xff;
		stream << "0x" << setfill('0') << setw(2) << hex << lower << ",";
		stream << "0x" << setfill('0') << setw(2) << hex << upper;
		stream << ",			// " << sanitizeComment(comment) << endl;
		sizeBytes += 2;
	}
	virtual void Comment(const string& comment) override
	{
		if(numColumns > 0)
		{
			numColumns = 0;
			stream << endl;
		}
		
		stream << "// " << sanitizeComment(comment) << endl;
	}

	string sanitizeComment(const string& comment)
	{
		string copy = comment;

		for (char& c : copy)
		{
			if (c == '\n')
			{
				c = ' ';
			}
		}
		return copy;
	}
	
	ostream& stream;
	int sizeBytes = 0;
	int numColumns = 0;
};

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout << "Usage " << argv[0] << " [file]" << endl;
		return 0;
	}
	
	FILE* fs = nullptr;
	fopen_s(&fs, argv[1], "r");
	
	if(!fs)
	{
		cerr << "Error opening " << argv[1] << endl;
		return -1;
	}
	
	Game game;
	Parser parser(game, fs);
	
	try
	{
		parser.Parse();

		ofstream outStream("IF/Generated/GameData.inc.h");
		HumanReadableGameDataWriter	writer(outStream);
		game.GenerateGameData(writer);
		
		cout << "Generated game data size: " << dec << writer.sizeBytes << " bytes" << endl;
		
		ofstream outStrings("IF/Generated/StringData.inc.h");
		HumanReadableGameDataWriter	stringWriter(outStrings);
		game.GetStringTable().Write(stringWriter);
	
		cout << "String table size: " << dec << stringWriter.sizeBytes << " bytes" << endl;
		cout << "Total size: " << dec << (writer.sizeBytes + stringWriter.sizeBytes) << " bytes" << endl;
	}
	catch(exception& e)
	{
		cerr << "Error: " << e.what() << endl;
	}

	fclose(fs);


	return 0;
}
