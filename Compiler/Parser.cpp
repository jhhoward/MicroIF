#include <exception>
#include "Parser.h"
#include "Game.h"
#include "Instructions.h"

using namespace std;

const char* TokenTypeNames[] =
{
	"null", "room", "item", "title", "description", "option", "go", "say", "give", "remove", "set", "unset", "flag", "on", "enter", "when", "not", "and", "or", "is", "have", "true", "false", "string", "identifier"
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

void Parser::ParseCondition(Room* room)
{
	room->instructions.push_back(new BasicInstruction(INSTRUCTION_CONDITION, "when"));
	NextToken();
	
	while(isParsing)
	{
		switch(tokenType)
		{
		case TokenType::Have:
			NextToken();
			Expect(TokenType::Identifier);
			room->instructions.push_back(new EvalHaveItemInstruction(tokenString));
			NextToken();
			break;
		case TokenType::Identifier:
			room->instructions.push_back(new EvalFlagInstruction(tokenString));
			NextToken();
			break;
		case TokenType::Not:
			room->instructions.push_back(new BasicInstruction(INSTRUCTION_NOT, "not"));
			NextToken();
			if(tokenType != TokenType::Identifier && tokenType != TokenType::Have)
				ThrowUnexpectedToken();
			break;
		case TokenType::And:
			room->instructions.push_back(new BasicInstruction(INSTRUCTION_AND, "and"));
			NextToken();
			if(tokenType != TokenType::Identifier && tokenType != TokenType::Not && tokenType != TokenType::Have)
				ThrowUnexpectedToken();
			break;
		case TokenType::Or:
			room->instructions.push_back(new BasicInstruction(INSTRUCTION_OR, "or"));
			NextToken();
			if(tokenType != TokenType::Identifier && tokenType != TokenType::Not && tokenType != TokenType::Have)
				ThrowUnexpectedToken();
			break;
		default:
			return;
		}
	}
}

void Parser::ParseEvent(Room* room)
{
	NextToken();
	
	switch(tokenType)
	{
		case TokenType::Enter:
		room->instructions.push_back(new EventInstruction((uint8_t)EVENT_ENTERROOM, "on enter"));
		NextToken();
		if(tokenType == TokenType::When)
		{
			ParseCondition(room);
		}
		ParseInstructions(room);
		break;
		
		default:
		ThrowUnexpectedToken();
		break;
	}
}

void Parser::ParseOption(Room* room)
{
	NextToken();
	
	if(tokenType == TokenType::String)
	{
		game.AddString(tokenString);
		room->instructions.push_back(new OptionInstruction(tokenString));
	}
	else if(tokenType == TokenType::Identifier)
	{
		room->instructions.push_back(new OptionItemInstruction(tokenString));
	}
	else
	{
		ThrowUnexpectedToken();
	}
	
	NextToken();

	if(tokenType == TokenType::When)
	{
		ParseCondition(room);
	}
	
	ParseInstructions(room);
}
	
void Parser::ParseInstructions(Room* room)
{
	while(isParsing)
	{
		switch(tokenType)
		{
		case TokenType::Go:
			NextToken();
			Expect(TokenType::Identifier);
			room->instructions.push_back(new GoInstruction(tokenString));
			NextToken();
			break;
			
		case TokenType::Set:
			NextToken();
			Expect(TokenType::Identifier);
			room->instructions.push_back(new SetFlagInstruction(tokenString, true));
			NextToken();
			break;

		case TokenType::Unset:
			NextToken();
			Expect(TokenType::Identifier);
			room->instructions.push_back(new SetFlagInstruction(tokenString, false));
			NextToken();
			break;
			
		case TokenType::Give:
			NextToken();
			Expect(TokenType::Identifier);
			room->instructions.push_back(new SetItemStateInstruction(tokenString, true));
			NextToken();
			break;

		case TokenType::Remove:
			NextToken();
			Expect(TokenType::Identifier);
			room->instructions.push_back(new SetItemStateInstruction(tokenString, false));
			NextToken();
			break;
			
		case TokenType::Say:
			NextToken();
			Expect(TokenType::String);
			room->instructions.push_back(new SayInstruction(tokenString));
			game.AddString(tokenString);
			NextToken();
			break;
			
		case TokenType::Room:
		case TokenType::Flag:
		case TokenType::Option:
		case TokenType::On:
			return;
		
		default:
			ThrowUnexpectedToken();
		}
	}
}

void Parser::ParseRoom()
{
	NextToken();
	Expect(TokenType::Identifier);
	
	Room* newRoom = game.NewRoom(tokenString);
	
	NextToken();
	
	while(isParsing)
	{
		switch(tokenType)
		{
			case TokenType::Room:
			case TokenType::Flag:
			case TokenType::Item:
				newRoom->instructions.push_back(new BasicInstruction(INSTRUCTION_NULL, "end"));
				return;
				
			case TokenType::Description:
				NextToken();
				Expect(TokenType::String);
				newRoom->description = tokenString;
				game.AddString(newRoom->description);
				NextToken();
				break;
			case TokenType::Title:
				NextToken();
				Expect(TokenType::String);
				newRoom->title = tokenString;
				game.AddString(newRoom->title);
				NextToken();
				break;
			case TokenType::Option:
				ParseOption(newRoom);
				break;
			case TokenType::On:
				ParseEvent(newRoom);
				break;
				
			default:
				ThrowUnexpectedToken();
		}
	}
	
	newRoom->instructions.push_back(new BasicInstruction(INSTRUCTION_NULL, "end"));
}

void Parser::ParseItem()
{
	NextToken();
	Expect(TokenType::Identifier);
	
	Item* newItem = game.NewItem(tokenString);
	
	NextToken();
	
	while(isParsing)
	{
		switch(tokenType)
		{
			case TokenType::Room:
			case TokenType::Flag:
			case TokenType::Item:
				return;
				
			case TokenType::Description:
				NextToken();
				Expect(TokenType::String);
				newItem->description = tokenString;
				game.AddString(newItem->description);
				NextToken();
				break;
			case TokenType::Title:
				NextToken();
				Expect(TokenType::String);
				newItem->title = tokenString;
				game.AddString(newItem->title);
				NextToken();
				break;
				
			default:
				ThrowUnexpectedToken();
		}
	}
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
