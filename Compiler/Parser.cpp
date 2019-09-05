#include <exception>
#include "Parser.h"
#include "Game.h"
#include "Instructions.h"

using namespace std;

const char* TokenTypeNames[] =
{
	"null", "room", "item", "title", "description", "option", "go", "say", "give", "remove", "set", "unset", "flag", "on", "enter", "use", "when", "not", "and", "or", "is", "have", "true", "false", "string", "identifier"
};

void Parser::ThrowUnexpectedToken()
{
	throw exception(("Unexpected token: " + string(TokenTypeNames[(int)tokenType]) + " [" + tokenString + "] at line " + to_string(parseLine)).c_str());
}

void Parser::Expect(TokenType expected)
{
	if(expected != tokenType)
		ThrowUnexpectedToken();
}

bool Parser::NextToken()
{
	if(feof(fs))
	{
		isParsing = false;
		return false;
	}
	
	constexpr int maxTokenLength = 1024;
	char tokenBuffer[maxTokenLength];
	
	int tokenLength = 0;
	tokenBuffer[0] = '\0';
	bool isReading = true;
	bool isSteppingOverComment = false;
	
	tokenType = TokenType::None;
	
	while(!feof(fs) && isReading)
	{
		int character = fgetc(fs);
		
		if(character == '\n')
		{
			parseLine++;
			isSteppingOverComment = false;
		}
		
		if(!isSteppingOverComment)
		{
			switch(character)
			{
				case '#':
				isSteppingOverComment = true;
				break;
				case -1:
				case ' ':
				case '\n':
				case '\t':
				case '\r':
					if(tokenLength == 0)
						continue;
					else
					{
						isReading = false;
					}
					break;
				case '"':
					if(tokenLength == 0)
					{
						isReading = false;
						tokenType = TokenType::String;
						bool parsingString = true;
						
						while (!feof(fs) && parsingString)
						{
							// Keep getting characters until end of string
							while (!feof(fs))
							{
								character = fgetc(fs);
								if (character == '"')
								{
									break;
								}
								tokenBuffer[tokenLength++] = (char)character;
							}

							// See if we need to concatenate with another string
							{
								long int filePosition = ftell(fs);
								while (!feof(fs))
								{
									character = fgetc(fs);

									if (character == '"')
									{
										tokenBuffer[tokenLength++] = '\n';
										break;
									}

									if (character == ' ' || character == '\n' || character == '\t' || character == '\r')
									{
										continue;
									}

									parsingString = false;
									fseek(fs, filePosition, SEEK_SET);
									break;
								}
							}
						}
					}
					else
					{
						throw exception("Unmatched \"\n");
					}
					break;
				default:
					tokenBuffer[tokenLength++] = (char) character;
				break;
			}
		}
	}
	
	if(tokenLength == 0)
	{
		isParsing = false;
		return false;
	}
	
	tokenBuffer[tokenLength] = '\0';
	tokenString = string(tokenBuffer);
	
	if(tokenType == TokenType::None)
	{
		tokenType = TokenType::Identifier;
		
		for(int n = 0; n < (int) TokenType::NumTokenTypes; n++)
		{
			if(!strcmp(tokenBuffer, TokenTypeNames[n]))
			{
				tokenType = (TokenType)(n);
				break;
			}
		}
	}
	
	return true;
}

void Parser::ParseCondition(GameObject* gameObject)
{
	if (tokenType != TokenType::When)
		return;

	gameObject->instructions.push_back(new BasicInstruction(INSTRUCTION_CONDITION, "when"));
	NextToken();
	
	while(isParsing)
	{
		switch(tokenType)
		{
		case TokenType::Have:
			NextToken();
			Expect(TokenType::Identifier);
			gameObject->instructions.push_back(new EvalHaveItemInstruction(tokenString));
			NextToken();
			break;
		case TokenType::Identifier:
			gameObject->instructions.push_back(new EvalFlagInstruction(tokenString));
			NextToken();
			break;
		case TokenType::Not:
			gameObject->instructions.push_back(new BasicInstruction(INSTRUCTION_NOT, "not"));
			NextToken();
			if(tokenType != TokenType::Identifier && tokenType != TokenType::Have)
				ThrowUnexpectedToken();
			break;
		case TokenType::And:
			gameObject->instructions.push_back(new BasicInstruction(INSTRUCTION_AND, "and"));
			NextToken();
			if(tokenType != TokenType::Identifier && tokenType != TokenType::Not && tokenType != TokenType::Have)
				ThrowUnexpectedToken();
			break;
		case TokenType::Or:
			gameObject->instructions.push_back(new BasicInstruction(INSTRUCTION_OR, "or"));
			NextToken();
			if(tokenType != TokenType::Identifier && tokenType != TokenType::Not && tokenType != TokenType::Have)
				ThrowUnexpectedToken();
			break;
		default:
			return;
		}
	}
}

void Parser::ParseEvent(GameObject* gameObject)
{
	NextToken();
	
	switch(tokenType)
	{
		case TokenType::Enter:
		gameObject->instructions.push_back(new EventInstruction((uint8_t)EVENT_ENTERROOM, "on enter"));
		NextToken();
		ParseCondition(gameObject);
		ParseInstructions(gameObject);
		break;

		case TokenType::Use:
		{
			NextToken();
			string targetItem;
			if (gameObject->GetType() == GameObject::Type::Item)
			{
				targetItem = gameObject->variableName;
			}
			else
			{
				Expect(TokenType::Identifier);
				targetItem = tokenString;
				NextToken();
			}
			gameObject->instructions.push_back(new UseItemInstruction(targetItem));

			ParseCondition(gameObject);
			ParseInstructions(gameObject);
		}
		break;
		
		default:
		ThrowUnexpectedToken();
		break;
	}
}

void Parser::ParseOption(GameObject* room)
{
	NextToken();
	
	if(tokenType == TokenType::String)
	{
		game.AddString(tokenString);
		room->instructions.push_back(new OptionInstruction(tokenString));
	}
	else
	{
		ThrowUnexpectedToken();
	}
	
	NextToken();

	ParseCondition(room);
	
	ParseInstructions(room);
}
	
void Parser::ParseInstructions(GameObject* gameObject)
{
	while(isParsing)
	{
		switch(tokenType)
		{
		case TokenType::Go:
			NextToken();
			Expect(TokenType::Identifier);
			gameObject->instructions.push_back(new GoInstruction(tokenString));
			NextToken();
			break;
			
		case TokenType::Set:
			NextToken();
			Expect(TokenType::Identifier);
			gameObject->instructions.push_back(new SetFlagInstruction(tokenString, true));
			NextToken();
			break;

		case TokenType::Unset:
			NextToken();
			Expect(TokenType::Identifier);
			gameObject->instructions.push_back(new SetFlagInstruction(tokenString, false));
			NextToken();
			break;
			
		case TokenType::Give:
			NextToken();
			Expect(TokenType::Identifier);
			gameObject->instructions.push_back(new SetItemStateInstruction(tokenString, true));
			NextToken();
			break;

		case TokenType::Remove:
			NextToken();
			Expect(TokenType::Identifier);
			gameObject->instructions.push_back(new SetItemStateInstruction(tokenString, false));
			NextToken();
			break;
			
		case TokenType::Say:
			NextToken();
			Expect(TokenType::String);
			gameObject->instructions.push_back(new SayInstruction(tokenString));
			game.AddString(tokenString);
			NextToken();
			break;
			
		case TokenType::Room:
		case TokenType::Flag:
		case TokenType::Item:
		case TokenType::Option:
		case TokenType::On:
			return;
		
		default:
			ThrowUnexpectedToken();
		}
	}
}

void Parser::ParseGameObject(GameObject* gameObject)
{
	while (isParsing)
	{
		switch (tokenType)
		{
		case TokenType::Room:
		case TokenType::Flag:
		case TokenType::Item:
			gameObject->instructions.push_back(new BasicInstruction(INSTRUCTION_NULL, "end"));
			return;

		case TokenType::Description:
			NextToken();
			gameObject->instructions.push_back(new EventInstruction(EVENT_DESCRIPTIONATTRIBUTE, "description"));
			ParseCondition(gameObject);
			Expect(TokenType::String);
			game.AddString(tokenString);
			gameObject->instructions.push_back(new ReturnInstruction(tokenString));
			NextToken();
			break;
		case TokenType::Title:
			NextToken();
			gameObject->instructions.push_back(new EventInstruction(EVENT_NAMEATTRIBUTE, "title"));
			ParseCondition(gameObject);
			Expect(TokenType::String);
			game.AddString(tokenString);
			gameObject->instructions.push_back(new ReturnInstruction(tokenString));
			NextToken();
			break;
		case TokenType::Option:
			ParseOption(gameObject);
			break;
		case TokenType::On:
			ParseEvent(gameObject);
			break;

		default:
			ThrowUnexpectedToken();
		}
	}

	gameObject->instructions.push_back(new BasicInstruction(INSTRUCTION_NULL, "end"));
}

void Parser::ParseRoom()
{
	NextToken();
	Expect(TokenType::Identifier);
	
	Room* newRoom = game.NewRoom(tokenString);
	
	NextToken();

	ParseGameObject(newRoom);
}

void Parser::ParseItem()
{
	NextToken();
	Expect(TokenType::Identifier);
	
	Item* newItem = game.NewItem(tokenString);
	
	NextToken();

	ParseGameObject(newItem);
}

void Parser::ParseFlag()
{
	NextToken();
	Expect(TokenType::Identifier);
	string flagName = tokenString;
	NextToken();
	Expect(TokenType::Is);
	NextToken();
	
	bool defaultValue = false;
	
	if(tokenType == TokenType::True)
	{
		defaultValue = true;
	}
	else if(tokenType != TokenType::False)
	{
		ThrowUnexpectedToken();
	}
	NextToken();
	
	game.AddFlag(flagName, defaultValue);
}

void Parser::Parse()
{
	NextToken();
	
	while(isParsing)
	{
		//printf("[%s] : %s\n", TokenTypeNames[(int) tokenType], tokenString.c_str());
		
		switch(tokenType)
		{
		case TokenType::Flag:
			ParseFlag();
			break;
		case TokenType::Room:
			ParseRoom();
			break;
		case TokenType::Item:
			ParseItem();
			break;
		default:
			ThrowUnexpectedToken();
		}
	}
}
