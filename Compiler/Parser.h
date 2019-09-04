#pragma once

#include <string>

class Game;

enum class TokenType
{
	None,
	Room,
	Item,
	Title,
	Description,
	Option,
	Go,
	Say,
	Give,
	Remove,
	Set,
	Unset,
	Flag,
	On,
	Enter,
	When,
	Not,
	And,
	Or,
	Is,
	Have,
	True,
	False,
	String,
	Identifier,
	NumTokenTypes
};

class Parser
{
public:
	Parser(Game& inGame, FILE* inFS) : game(inGame), fs(inFS) {}
	
	void Parse();
	
private:
	bool NextToken();
	void ThrowUnexpectedToken();
	void Expect(TokenType tokenType);

	void ParseOption(class Room* room);
	void ParseEvent(class Room* room);
	void ParseCondition(class Room* room);
	void ParseInstructions(class Room* room);
	void ParseRoom();
	void ParseFlag();
	void ParseItem();

	std::string tokenString;
	TokenType tokenType;
	int parseLine = 1;
	bool isParsing = true;
	
	Game& game;
	FILE* fs;
};
