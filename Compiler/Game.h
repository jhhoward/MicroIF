#pragma once

#include <list>
#include <map>
#include <set>
#include <string>
#include <stdint.h>

class Instruction;
class StringTable;

class GameDataWriter
{
public:
	virtual void Write(uint8_t value) = 0;
	virtual void Write(uint8_t value, const std::string& comment) = 0;
	virtual void Write(uint8_t value, uint8_t param, const std::string& comment) = 0;
	virtual void Write(uint8_t value, uint16_t param, const std::string& comment) = 0;
	virtual void Write(uint16_t value, const std::string& comment) = 0;
	virtual void Comment(const std::string& comment) = 0;
};

class GameObject
{
public:
	enum class Type
	{
		Room,
		Item
	};

	GameObject(const std::string& identifier) : variableName(identifier) {}
	virtual ~GameObject();

	virtual Type GetType() = 0;

	std::string variableName;
	int index;
	std::list<Instruction*> instructions;

	// Used for writing out data
	int instructionStartLocation;
};

class Room : public GameObject
{
public:
	Room(const std::string& identifier) : GameObject(identifier) {}

	virtual Type GetType() { return Type::Room; }
};

class Item : public GameObject
{
public:
	Item(const std::string& identifier) : GameObject(identifier) {}

	virtual Type GetType() { return Type::Item; }
};

class Flag
{
public:
	Flag(const std::string& identifier, bool inDefaultValue) : variableName(identifier), defaultValue(inDefaultValue) {}
	std::string variableName;
	bool defaultValue;
	int index;
};

class Game
{
public:
	~Game();

	Room* GetRoom(const std::string& name);
	Room* NewRoom(const std::string& name);
	Item* GetItem(const std::string& name);
	Item* NewItem(const std::string& name);
	void AddFlag(const std::string& flagName, bool defaultValue);
	int GetFlagIndex(const std::string& flagName);
	void AddString(const std::string& inString);
	void GenerateGameData(class GameDataWriter& writer);
	StringTable& GetStringTable() { return *stringTable; }

	Room* startingRoom = nullptr;
	
private:
	std::map<std::string, GameObject*> objectMap;
	std::list<GameObject*> objectList;

	//std::map<std::string, Room*> rooms;	
	//std::list<Room*> roomsList;
	//std::map<std::string, Item*> items;
	//std::list<Item*> itemsList;

	std::map<std::string, Flag*> flags;
	//std::list<Flag*> flagsList;
	std::set<std::string> stringSet;
	
	StringTable* stringTable = nullptr;

};
