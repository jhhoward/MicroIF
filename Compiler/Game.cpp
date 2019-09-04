#include <memory.h>
#include "Game.h"
#include "StringTable.h"
#include "Instructions.h"

using namespace std;

Room::~Room()
{
	for(Instruction* ins : instructions)
	{
		delete ins;
	}
}

Room* Game::GetRoom(const string& name)
{
	auto it = rooms.find(name);
	if(it == rooms.end())
	{
		throw exception(("Room with name " + name + " not found!").c_str());
	}
	return rooms[name];
}

Room* Game::NewRoom(const string& name)
{
	if(rooms.find(name) != rooms.end())
	{
		throw exception(("Room with name " + name + " already exists!").c_str());
	}
	Room* room = new Room(name, rooms.size());
	rooms[name] = room;
	roomsList.push_back(room);
	return room;
}

Item* Game::GetItem(const string& name)
{
	auto it = items.find(name);
	if(it == items.end())
	{
		throw exception(("Item with name " + name + " not found!").c_str());
	}
	return items[name];
}

Item* Game::NewItem(const string& name)
{
	if(items.find(name) != items.end())
	{
		throw exception(("Item with name " + name + " already exists!").c_str());
	}
	Item* item = new Item(name, items.size());
	items[name] = item;
	itemsList.push_back(item);
	return item;
}

int Game::GetFlagIndex(const std::string& name)
{
	auto it = flags.find(name);
	if(it == flags.end())
	{
		throw exception(("Flag with name " + name + " not found!").c_str());
	}
	return it->second->index + itemsList.size();
}

void Game::AddFlag(const string& flagName, bool defaultValue)
{
	if(flags.find(flagName) != flags.end())
	{
		throw exception(("Flag with name " + flagName + " already exists!").c_str());
	}
	Flag* flag = new Flag(flagName, defaultValue, flags.size());
	flags[flagName] = flag;
	flagsList.push_back(flag);
}

void Game::AddString(const string& inString)
{
	stringSet.insert(inString);
}

Game::~Game()
{
	for(Room* room : roomsList)
	{
		delete room;
	}
	for(Flag* flag : flagsList)
	{
		delete flag;
	}
	
	if(stringTable)
		delete stringTable;
}

class DataSizeCalculator : public GameDataWriter
{
public:
	virtual void Write(uint8_t value) { size ++; }
	virtual void Write(uint8_t value, const std::string& comment) { size ++; }
	virtual void Write(uint8_t value, uint8_t param, const std::string& comment) { size += 2; }
	virtual void Write(uint8_t value, uint16_t param, const std::string& comment) { size += 3; }
	virtual void Write(uint16_t value, const std::string& comment) { size += 2; }
	virtual void Comment(const std::string& comment) { }

	int size = 0;
};

void Game::GenerateGameData(GameDataWriter& writer)
{
	stringTable = new StringTable(stringSet);
	
	if(flagsList.size() + itemsList.size() > 256)
	{
		throw exception("Too many flags + items, reached 256 limit!");
	}
	
	constexpr int numFlagBytes = 256 / 8;
	uint8_t defaultValues[numFlagBytes];
	memset(defaultValues, 0, numFlagBytes);
	
	for(Flag* flag : flagsList)
	{
		if(flag->defaultValue)
		{
			int index = flag->index + itemsList.size();
			int byteIndex = index / 8;
			int bitMask = 1 << (index % 8);
			defaultValues[byteIndex] |= bitMask;
		}
	}
	
	writer.Comment("Default flag values");
	for(int n = 0; n < numFlagBytes; n++)
	{
		writer.Write(defaultValues[n]);
	}
	
	writer.Write((uint8_t) items.size(), "num items");
	writer.Write((uint8_t) rooms.size(), "num rooms");
	
	constexpr int sizeOfRoomHeader = 6;
	constexpr int sizeOfItemHeader = 4; 
	constexpr int sizeOfDataHeader = 2 + numFlagBytes;
	
	for(Item* item : itemsList)
	{
		writer.Comment(item->variableName);
		writer.Write(stringTable->GetIndex(item->title), "\"" + item->title + "\"");
		writer.Write(stringTable->GetIndex(item->description), "\"" + item->description + "\"");
	}
	
	int roomDataStartLocation = sizeOfDataHeader + roomsList.size() * sizeOfRoomHeader + itemsList.size() * sizeOfItemHeader;
	
	int instructionStartLocation = roomDataStartLocation;
	
	// Calculate instructions size for each room
	for(Room* room : roomsList)
	{
		DataSizeCalculator calculator;
		
		for(Instruction* instruction : room->instructions)
		{
			instruction->Emit(*this, calculator);
		}
		room->instructionByteSize = calculator.size;
		room->instructionStartLocation = instructionStartLocation;
		instructionStartLocation += room->instructionByteSize;
	}
	
	for(Room* room : roomsList)
	{
		writer.Comment(room->variableName + " [" + to_string(room->index) + "]");
		writer.Write(stringTable->GetIndex(room->title), "\"" + room->title + "\"");
		writer.Write(stringTable->GetIndex(room->description), "\"" + room->description + "\"");
		writer.Write((uint16_t) room->instructionStartLocation, "data offset");
	}
	
	for(Room* room : roomsList)
	{
		writer.Comment(room->variableName + " data");
		for(Instruction* instruction : room->instructions)
		{
			instruction->Emit(*this, writer);
		}
	}
}

