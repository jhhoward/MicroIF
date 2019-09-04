#include <stdint.h>
#include "InstructionCodes.h"
#include "VM.h"

VM::VM(DataArchive& inGameData, DataArchive& inStringData) : gameData(inGameData), stringData(inStringData)
{
	Reset();
}

void VM::Reset()
{
	currentRoomIndex = 0;
	
	gameData.Seek(0);
	for(int n = 0; n < 256 / 8; n++)
	{
		flagState[n] = gameData.ReadByte();
	}

	if(Execute(ExecutionMode::ExecuteEvent, EVENT_ENTERROOM))
	{
		Execute(ExecutionMode::EnumerateOptions);
		state = State::WaitingForOptionChoice;
	}
}

void VM::GetTextInternal(StringIndex stringIndex, char** buffer)
{
	uint16_t oldAddress = stringData.Tell();

	stringData.Seek(stringIndex);

	while (uint8_t c = stringData.ReadByte())
	{
		if (c < 128)
		{
			**buffer = c;
			(*buffer)++;
		}
		else
		{
			GetCodedPhrase(c - 128, buffer);
		}
	}
	**buffer = '\0';

	stringData.Seek(oldAddress);
}

void VM::GetCodedPhrase(uint8_t index, char** buffer)
{
	uint16_t oldAddress = stringData.Tell();

	stringData.Seek(0);

	while (index > 0)
	{
		if (stringData.ReadByte() == '\0')
		{
			index--;
		}
	}

	GetTextInternal(stringData.Tell(), buffer);

	stringData.Seek(oldAddress);
}

void VM::GetText(StringIndex stringIndex, char* buffer)
{
	if (stringIndex == 0xffff)
	{
		buffer[0] = '\0';
		return;
	}
	GetTextInternal(stringIndex, &buffer);
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

bool VM::ExecuteInstructions(bool stepOver)
{
	bool parsing = true;
	
	while(parsing)
	{
		uint8_t instruction = gameData.PeekByte();
		
		switch(instruction)
		{
			case INSTRUCTION_GO:
			{
				gameData.NextByte();
				uint8_t nextRoom = gameData.ReadByte();
				if(!stepOver)
				{
					currentRoomIndex = nextRoom;
					bool completed = Execute(ExecutionMode::ExecuteEvent, EVENT_ENTERROOM);
					if (completed)
					{
						Execute(ExecutionMode::EnumerateOptions);
					}
				}
			}
			break;
			
			case INSTRUCTION_SAY:
			{
				gameData.NextByte();
				uint16_t messageIndex = gameData.ReadWord();
				if(!stepOver)
				{
					currentMessageText = messageIndex;
					state = State::ShowingMessage;
					return false;
				}
			}
			break;
			
			case INSTRUCTION_SETFLAG:
			{
				gameData.NextByte();
				uint8_t flag = gameData.ReadByte();
				if(!stepOver)
				{
					SetFlag(flag, true);
				}
			}
			break;

			case INSTRUCTION_UNSETFLAG:
			{
				gameData.NextByte();
				uint8_t flag = gameData.ReadByte();
				if(!stepOver)
				{
					SetFlag(flag, false);
				}
			}
			break;
			
			default:
			return true;
		}
	}
	
	return true;
}

bool VM::EvaluateCondition()
{
	bool parsing = true;
	bool negate = false;
	bool result = true;
	uint8_t logicOp = INSTRUCTION_NULL;

	while(parsing)
	{
		uint8_t instruction = gameData.PeekByte();
		
		switch(instruction)
		{
			case INSTRUCTION_EVALUATEFLAG:
			{
				gameData.NextByte();
				uint8_t flagIndex = gameData.PeekByte();
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
		
		gameData.NextByte();
	}
	
	return result;
}

#include <stdlib.h>

bool VM::Execute(ExecutionMode mode, uint8_t param)
{
	RoomHeader roomHeader = GetRoomHeader(currentRoomIndex);
	
	gameData.Seek(roomHeader.dataPointer);
	
	bool parsing = true;
	uint8_t optionIndex = 0;
	
	while(parsing)
	{
		uint8_t instruction = gameData.ReadByte();
		
		switch(instruction)
		{
			case INSTRUCTION_NULL:
				parsing = false;
				break;
			case INSTRUCTION_EVENT:
				{
					uint8_t eventId = gameData.ReadByte();
					bool possible = true;					
					if(gameData.PeekByte() == INSTRUCTION_CONDITION)
					{
						gameData.NextByte();
						possible = EvaluateCondition();
					}
					bool stepOver = !possible || mode != ExecutionMode::ExecuteEvent || param != eventId;
					bool completed = ExecuteInstructions(stepOver);
					
					if(!completed)
					{
						return false;
					}
					
					if(!stepOver)
					{
						return true;
					}
				}
				break;
			case INSTRUCTION_OPTION:
				{
					uint16_t text = gameData.ReadWord();
					bool visible = true;
					if(gameData.PeekByte() == INSTRUCTION_CONDITION)
					{
						gameData.NextByte();
						visible = EvaluateCondition();
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
					bool completed = ExecuteInstructions(stepOver);
					if(!completed)
					{
						return false;
					}
					
					if(!stepOver)
					{
						return true;
					}					
				}
				break;
			case INSTRUCTION_ITEMOPTION:
				{
					uint8_t item = gameData.ReadByte();
					bool possible = true;
					if(gameData.PeekByte() == INSTRUCTION_CONDITION)
					{
						gameData.NextByte();
						possible = EvaluateCondition();
					}
					bool stepOver = !possible || mode != ExecutionMode::ExecuteItemOption || param != item;
					bool completed = ExecuteInstructions(stepOver);
					
					if(!completed)
					{
						return false;
					}
					
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
	
	return true;
}

void VM::DismissMessage()
{
	if(state == State::ShowingMessage)
	{
		bool completed = ExecuteInstructions(false);
		if(completed)
		{
			Execute(ExecutionMode::EnumerateOptions);
			state = State::WaitingForOptionChoice;
		}
	}
}

void VM::ChooseOption(uint8_t index)
{
	if(state == State::WaitingForOptionChoice)
	{
		Execute(ExecutionMode::ExecuteOption, index);
	}
}

void VM::LookAround()
{
	if (state == State::WaitingForOptionChoice)
	{
		currentMessageText = GetCurrentRoomHeader().description;
		state = State::ShowingMessage;
	}
}

void VM::UseItem(ItemIndex item)
{
	if(state == State::WaitingForOptionChoice)
	{
		if(GetFlag(item) && item < GetDataHeader().numItems)
		{
			Execute(ExecutionMode::ExecuteItemOption, item);
		}
	}
}

ItemHeader VM::GetItemHeader(ItemIndex item)
{
	ItemHeader result;
	constexpr int itemHeaderStart = 34;
	constexpr int itemHeaderSize = 4;
	uint16_t oldAddress = gameData.Tell();

	gameData.Seek(itemHeaderStart + itemHeaderSize * item);
	result.title = gameData.ReadWord();
	result.description = gameData.ReadWord();

	gameData.Seek(oldAddress);

	return result;
}

uint8_t VM::GetNumInventoryItems()
{
	DataHeader header = GetDataHeader();
	uint8_t count = 0;

	for (uint8_t n = 0; n < header.numItems; n++)
	{
		if (GetFlag(n))
		{
			count++;
		}
	}

	return count;
}

ItemIndex VM::GetInventoryItem(uint8_t slotIndex)
{
	DataHeader header = GetDataHeader();
	uint8_t count = 0;

	for (uint8_t n = 0; n < header.numItems; n++)
	{
		if (GetFlag(n))
		{
			if (count == slotIndex)
			{
				return n;
			}
			count++;
		}
	}

	return 0;
}

void VM::UnlockAllItems()
{
	DataHeader header = GetDataHeader();

	for (uint8_t n = 0; n < header.numItems; n++)
	{
		SetFlag(n, true);
	}
}

DataHeader VM::GetDataHeader()
{
	DataHeader result;
	constexpr int dataHeaderStart = 32;
	uint16_t oldAddress = gameData.Tell();

	gameData.Seek(dataHeaderStart);
	result.numItems = gameData.ReadByte();
	result.numRooms = gameData.ReadByte();

	gameData.Seek(oldAddress);

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
	uint16_t oldAddress = gameData.Tell();
	gameData.Seek(roomHeaderStart + roomHeaderSize * roomIndex);
	
	result.title = gameData.ReadWord();
	result.description = gameData.ReadWord();
	result.dataPointer = gameData.ReadWord();
	
	gameData.Seek(oldAddress);

	return result;
}


#if 0
#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;

int main(int argc, char* argv[])
{
	int roomIndex = 1;
	char buffer[256];
	
	VM vm(code, stringData);
	
	while(1)
	{
		switch(vm.GetState())
		{
			case VM::State::WaitingForOptionChoice:
			{
				RoomHeader room = vm.GetCurrentRoomHeader();
				vm.GetText(room.title, buffer);
				cout << buffer << endl;
				vm.GetText(room.description, buffer);
				cout << buffer << endl;
				
				for(uint8_t n = 0; n < vm.GetNumOptions(); n++)
				{
					vm.GetText(vm.GetOptionText(n), buffer);
					cout << (n + 1) << " : ";
					cout << buffer << endl;
				}
				
				int input = getchar();
				while(getchar() != '\n') {}
		
				int option = input - '1';
				if(option >= 0 && option < vm.GetNumOptions())
				{
					vm.ChooseOption((uint8_t) option);
				}
			}
			break;
			
			case VM::State::ShowingMessage:
			{
				vm.GetText(vm.GetCurrentMessageText(), buffer);
				cout << buffer << endl;
				getchar();
				vm.DismissMessage();
			}
			break;
		}
		
		/*
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
		}*/
	}
	
	return 0;
}
#endif