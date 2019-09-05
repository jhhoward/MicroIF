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
	Use,
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

	void ParseOption(class GameObject* room);
	void ParseEvent(class GameObject* room);
	void ParseCondition(class GameObject* room);
	void ParseInstructions(class GameObject* room);
	void ParseGameObject(GameObject* gameObject);

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
