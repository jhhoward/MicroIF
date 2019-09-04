#include <stdint.h>
#include "InstructionCodes.h"
#include "VM.h"

const uint8_t code[] =
{
	#include "../Generated/game.m"
};

const uint8_t stringData[] =
{
	#include "../Generated/strings.m"
};

VM::VM(const uint8_t* inGameData) : gameData(inGameData)
{
	Reset();
}

void VM::Reset()
{
	currentRoomIndex = 0;
	
	ScriptPointer ptr(gameData);
	for(int n = 0; n < 256 / 8; n++)
	{
		flagState[n] = ptr.ReadByte();
	}
}

#include <stdio.h>
#include <windows.h>

void PrintCodedPhrase(uint8_t index);

void PrintStringInternal(const uint8_t* ptr)
{
	while(*ptr)
	{
		if(*ptr < 128)
		{
			putchar(*ptr);
			Sleep(25);
		}
		else
		{
			PrintCodedPhrase((*ptr) - 128);
		}
		ptr++;
	}
}

void PrintCodedPhrase(uint8_t index)
{
	const uint8_t* ptr = stringData;
	
	while(index > 0)
	{
		if(*ptr == '\0')
		{
			index--;
		}
		ptr++;
	}
	
	PrintStringInternal(ptr);
}

void PrintString(int16_t stringIndex)
{
	PrintStringInternal(stringData + stringIndex);
}

void VM::PrintOptions()
{
	Execute(ExecutionMode::EnumerateOptions);
	
	for(int n = 0; n < numOptions; n++)
	{
		printf("%d : ", n + 1);
		PrintString(optionText[n]);
		printf("\n");
	}
	
	DataHeader dataHeader = GetDataHeader();
	for(int n = 0; n < dataHeader.numItems; n++)
	{
		if(GetFlag((uint8_t)n))
		{
			ItemHeader item = GetItemHeader(n);
			printf("%c : ", (char)('a' + n));
			PrintString(item.title);
			printf("\n");
		}
	}
}

bool VM::GetFlag(uint8_t index)
{
	int byteIndex = index / 8;
	uint8_t mask = 1 << (index % 8);
	
	return (flagState[byteIndex] & mask) != 0;
}

void VM::SetFlag(uint8_t index, bool value)
{
	int byteIndex = index / 8;
	uint8_t mask = 1 << (index % 8);

	if(value)
	{
		flagState[byteIndex] |= mask;
	}
	else
	{
		flagState[byteIndex] &= ~mask;
	}
}

void VM::ExecuteOption(ScriptPointer& ptr, bool stepOver)
{
	bool parsing = true;
	
	while(parsing)
	{
		uint8_t instruction = ptr.PeekByte();
		
		switch(instruction)
		{
			case INSTRUCTION_GO:
			{
				ptr.NextByte();
				uint8_t nextRoom = ptr.ReadByte();
				if(!stepOver)
				{
					currentRoomIndex = nextRoom;
					Execute(ExecutionMode::ExecuteEvent, EVENT_ENTERROOM);
				}
			}
			break;
			
			case INSTRUCTION_SAY:
			{
				ptr.NextByte();
				uint16_t messageIndex = ptr.ReadWord();
				if(!stepOver)
				{
					PrintString(messageIndex);
					printf("\n");
				}
			}
			break;
			
			case INSTRUCTION_SETFLAG:
			{
				ptr.NextByte();
				uint8_t flag = ptr.ReadByte();
				if(!stepOver)
				{
					SetFlag(flag, true);
				}
			}
			break;

			case INSTRUCTION_UNSETFLAG:
			{
				ptr.NextByte();
				uint8_t flag = ptr.ReadByte();
				if(!stepOver)
				{
					SetFlag(flag, false);
				}
			}
			break;
			
			default:
			return;
		}
	}
}

bool VM::EvaluateCondition(ScriptPointer& ptr)
{
	bool parsing = true;
	bool negate = false;
	bool result = true;
	uint8_t logicOp = INSTRUCTION_NULL;


	while(parsing)
	{
		uint8_t instruction = ptr.PeekByte();
		
		switch(instruction)
		{
			case INSTRUCTION_EVALUATEFLAG:
			{
				ptr.NextByte();
				uint8_t flagIndex = ptr.PeekByte();
				bool flagValue = GetFlag(flagIndex);
				if(negate)
				{
					flagValue = !flagValue;
					negate = false;
				}
				
				switch(logicOp)
				{
					case INSTRUCTION_NULL:
					result = flagValue;
					break;
					case INSTRUCTION_AND:
					result = result && flagValue;
					break;
					case INSTRUCTION_OR:
					result = result || flagValue;
					break;
				}
				logicOp = INSTRUCTION_NULL;
			}
			break;
			
			case INSTRUCTION_NOT:
			negate = true;
			break;
			
			case INSTRUCTION_AND:
			logicOp = INSTRUCTION_AND;
			break;
			
			case INSTRUCTION_OR:
			logicOp = INSTRUCTION_OR;
			break;
			
			default:
			return result;
		}
		
		ptr.NextByte();
	}
	
	return result;
}

#include <stdlib.h>

bool VM::Execute(ExecutionMode mode, uint8_t param)
{
	RoomHeader roomHeader = GetRoomHeader(currentRoomIndex);
	
	ScriptPointer ptr = roomHeader.dataPointer;
	bool parsing = true;
	uint8_t optionIndex = 0;
	
	while(parsing)
	{
		uint8_t instruction = ptr.ReadByte();
		
		switch(instruction)
		{
			case INSTRUCTION_NULL:
				parsing = false;
				break;
			case INSTRUCTION_EVENT:
				{
					uint8_t eventId = ptr.ReadByte();
					bool possible = true;					
					if(ptr.PeekByte() == INSTRUCTION_CONDITION)
					{
						ptr.NextByte();
						possible = EvaluateCondition(ptr);
					}
					bool stepOver = !possible || mode != ExecutionMode::ExecuteEvent || param != eventId;
					ExecuteOption(ptr, stepOver);
					if(!stepOver)
					{
						return true;
					}
				}
				break;
			case INSTRUCTION_OPTION:
				{
					uint16_t text = ptr.ReadWord();
					bool visible = true;
					if(ptr.PeekByte() == INSTRUCTION_CONDITION)
					{
						ptr.NextByte();
						visible = EvaluateCondition(ptr);
					}
					bool stepOver = !visible || mode != ExecutionMode::ExecuteOption || param != optionIndex;
					if (visible)
					{
						if (mode == ExecutionMode::EnumerateOptions)
						{
							optionText[optionIndex] = text;
						}
						optionIndex++;
					}
					ExecuteOption(ptr, stepOver);
					if(!stepOver)
					{
						return true;
					}					
				}
				break;
			case INSTRUCTION_ITEMOPTION:
				{
					uint8_t item = ptr.ReadByte();
					bool possible = true;
					if(ptr.PeekByte() == INSTRUCTION_CONDITION)
					{
						ptr.NextByte();
						possible = EvaluateCondition(ptr);
					}
					bool stepOver = !possible || mode != ExecutionMode::ExecuteItemOption || param != item;
					ExecuteOption(ptr, stepOver);
					if(!stepOver)
					{
						return true;
					}
				}
				break;
			default:
				exit(0);
				break;
		}
	}
	
	if(mode == ExecutionMode::EnumerateOptions)
	{
		numOptions = optionIndex;
	}
	
	return false;
}

void VM::ChooseOption(uint8_t index)
{
	Execute(ExecutionMode::ExecuteOption, index);
}

void VM::UseItem(uint8_t index)
{
	if(GetFlag(index) && index < GetDataHeader().numItems)
	{
		Execute(ExecutionMode::ExecuteItemOption, index);
	}
}

ItemHeader VM::GetItemHeader(uint8_t itemIndex)
{
	ItemHeader result;
	constexpr int itemHeaderStart = 34;
	constexpr int itemHeaderSize = 4;
	ScriptPointer headerPointer(gameData, itemHeaderStart + itemHeaderSize * itemIndex);
	
	result.title = headerPointer.ReadWord();
	result.description = headerPointer.ReadWord();
	
	return result;
}

DataHeader VM::GetDataHeader()
{
	DataHeader result;
	constexpr int dataHeaderStart = 32;
	ScriptPointer headerPointer(gameData, dataHeaderStart);
	result.numItems = headerPointer.ReadByte();
	result.numRooms = headerPointer.ReadByte();
	
	return result;
}

RoomHeader VM::GetRoomHeader(uint8_t roomIndex)
{
	DataHeader dataHeader = GetDataHeader();
	RoomHeader result;
	constexpr int roomHeaderSize = 6;
	constexpr int itemHeaderStart = 34;
	constexpr int itemHeaderSize = 4;
	int roomHeaderStart = itemHeaderStart + itemHeaderSize * dataHeader.numItems;
	ScriptPointer headerPointer(gameData, roomHeaderStart + roomHeaderSize * roomIndex);
	
	result.title = headerPointer.ReadWord();
	result.description = headerPointer.ReadWord();
	result.dataPointer = ScriptPointer(gameData, headerPointer.ReadWord());
	
	return result;
}


#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	int roomIndex = 1;
	
	VM vm(code);
	
	while(1)
	{
		RoomHeader roomHeader = vm.GetRoomHeader(vm.GetCurrentRoomIndex());
		
		cout << endl << endl;
		PrintString(roomHeader.title);
		cout << endl;
		PrintString(roomHeader.description);
		cout << endl;

		vm.PrintOptions();
		
		int input = getchar();
		
		while(getchar() != '\n') {}
		
		int option = input - '1';
		if(option >= 0 && option < vm.GetNumOptions())
		{
			vm.ChooseOption((uint8_t) option);
		}
		
		int itemOption = input - 'a';
		if(itemOption >= 0 && itemOption < vm.GetDataHeader().numItems)
		{
			vm.UseItem((uint8_t) itemOption);
		}
	}
	
	return 0;
}
