#include <memory.h>
#include "Game.h"
#include "StringTable.h"
#include "Instructions.h"

using namespace std;

GameObject::~GameObject()
{
	for(Instruction* ins : instructions)
	{
		delete ins;
	}
}

Room* Game::GetRoom(const string& name)
{
	auto it = objectMap.find(name);
	if(it == objectMap.end())
	{
		throw exception(("Room with name " + name + " not found!").c_str());
	}

	GameObject* obj = it->second;
	if (obj->GetType() == GameObject::Type::Room)
		return (Room*)(obj);
	else
	{
		throw exception(("Object with name " + name + " is not a room!").c_str());
	}	
}

Room* Game::NewRoom(const string& name)
{
	if(objectMap.find(name) != objectMap.end())
	{
		throw exception(("Object with name " + name + " already exists!").c_str());
	}

	Room* room = new Room(name);
	objectMap[name] = room;

	if (!startingRoom)
	{
		startingRoom = room;
	}
	return room;
}

Item* Game::GetItem(const string& name)
{
	auto it = objectMap.find(name);
	if (it == objectMap.end())
	{
		throw exception(("Item with name " + name + " not found!").c_str());
	}

	GameObject* obj = it->second;
	if (obj->GetType() == GameObject::Type::Item)
		return (Item*)(obj);
	else
	{
		throw exception(("Object with name " + name + " is not an item!").c_str());
	}
}

Item* Game::NewItem(const string& name)
{
	if (objectMap.find(name) != objectMap.end())
	{
		throw exception(("Object with name " + name + " already exists!").c_str());
	}

	Item* item = new Item(name);
	objectMap[name] = item;
	return item;
}

int Game::GetFlagIndex(const std::string& name)
{
	auto it = flags.find(name);
	if(it == flags.end())
	{
		throw exception(("Flag with name " + name + " not found!").c_str());
	}
	return it->second->index;
}

void Game::AddFlag(const string& flagName, bool defaultValue)
{
	if(flags.find(flagName) != flags.end())
	{
		throw exception(("Flag with name " + flagName + " already exists!").c_str());
	}
	Flag* flag = new Flag(flagName, defaultValue);
	flags[flagName] = flag;
}

void Game::AddString(const string& inString)
{
	stringSet.insert(inString);
}

Game::~Game()
{
	for(auto& it : objectMap)
	{
		delete it.second;
	}
	for(auto& it: flags)
	{
		delete it.second;
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

	// Generate object list
	int numItems = 0;
	objectList.clear();

	// First add items, then add rooms, generate indices for each object type
	for (auto& it : objectMap)
	{
		if (it.second->GetType() == GameObject::Type::Item)
		{
			it.second->index = objectList.size();
			objectList.push_back(it.second);
			numItems++;
		}
	}
	for (auto& it : objectMap)
	{
		if (it.second->GetType() == GameObject::Type::Room)
		{
			it.second->index = objectList.size();
			objectList.push_back(it.second);
		}
	}

	// Assign flag indices
	int flagIndex = numItems;
	for (auto& it : flags)
	{
		it.second->index = flagIndex++;
	}

	if(flags.size() + numItems > 256)
	{
		throw exception("Too many flags + items, reached 256 limit!");
	}
	if (objectList.size() >= 256)
	{
		throw exception("Too many objects, reached 256 limit!");
	}
	
	constexpr int numFlagBytes = 256 / 8;
	uint8_t defaultValues[numFlagBytes];
	memset(defaultValues, 0, numFlagBytes);
	
	for(auto& it : flags)
	{
		Flag* flag = it.second;
		if(flag->defaultValue)
		{
			int byteIndex = flag->index / 8;
			int bitMask = 1 << (flag->index % 8);
			defaultValues[byteIndex] |= bitMask;
		}
	}
	
	// Write default flag values
	writer.Comment("Default flag values");
	for(int n = 0; n < numFlagBytes; n++)
	{
		writer.Write(defaultValues[n]);
	}
	
	// Write object header
	writer.Write((uint8_t)objectList.size(), "num objects");
	writer.Write((uint8_t)numItems, "num items");
	if (!startingRoom)
	{
		throw exception("No room to start in!");
	}
	writer.Write((uint8_t)startingRoom->index, "starting room : " + startingRoom->variableName);	
	
	constexpr int sizeOfObjectHeader = 2;
	constexpr int sizeOfDataHeader = 3 + numFlagBytes;
	
	int objectDataStartLocation = sizeOfDataHeader + objectList.size() * sizeOfObjectHeader;
	int instructionStartLocation = objectDataStartLocation;
	
	// Calculate instructions size for each object
	for(GameObject* obj : objectList)
	{
		DataSizeCalculator calculator;
		
		for(Instruction* instruction : obj->instructions)
		{
			instruction->Emit(*this, calculator);
		}
		obj->instructionStartLocation = instructionStartLocation;
		instructionStartLocation += calculator.size;
	}
	
	// Write list of game object pointers
	for (GameObject* obj : objectList)
	{
		writer.Comment(obj->variableName + " [" + to_string(obj->index) + "]");
		writer.Write((uint16_t) obj->instructionStartLocation, "data offset");
	}
	
	// Write instructions for each game object
	for (GameObject* obj : objectList)
	{
		writer.Comment(obj->variableName + " data");
		for(Instruction* instruction : obj->instructions)
		{
			instruction->Emit(*this, writer);
		}
	}
}

