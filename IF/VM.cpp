#include <stdint.h>
#include "InstructionCodes.h"
#include "VM.h"

VM::VM(DataArchive& inGameData, DataArchive& inStringData) : gameData(inGameData), stringData(inStringData)
{
	Reset();
}

void VM::Reset()
{
	currentRoomIndex = ReadByteAtAddress(startingRoomDataLocation);
	
	gameData.Seek(0);
	for(int n = 0; n < 256 / 8; n++)
	{
		flagState[n] = gameData.ReadByte();
	}

	lastExecutionResult = Execute(currentRoomIndex, ExecutionMode::ExecuteEvent, EVENT_ENTERROOM);

	if(lastExecutionResult.DidComplete())
	{
		Execute(currentRoomIndex, ExecutionMode::EnumerateOptions);
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

VM::ExecutionResult VM::ExecuteInstructions(bool stepOver)
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
				if(EvaluateCondition() && !stepOver)
				{
					currentRoomIndex = nextRoom;
					ExecutionResult goResult = Execute(currentRoomIndex, ExecutionMode::ExecuteEvent, EVENT_ENTERROOM);
					if (!goResult.DidYield())
					{
						Execute(currentRoomIndex, ExecutionMode::EnumerateOptions);
					}
					else
					{
						return goResult;
					}
				}
			}
			break;
			
			case INSTRUCTION_SAY:
			{
				gameData.NextByte();
				uint16_t messageIndex = gameData.ReadWord();
				if(EvaluateCondition() && !stepOver)
				{
					currentMessageText = messageIndex;
					state = State::ShowingMessage;
					return ExecutionResult(ExecutionResult::Type::Yielded, gameData.Tell());
				}
			}
			break;
			
			case INSTRUCTION_SETFLAG:
			{
				gameData.NextByte();
				uint8_t flag = gameData.ReadByte();
				if(EvaluateCondition() && !stepOver)
				{
					SetFlag(flag, true);
				}
			}
			break;

			case INSTRUCTION_UNSETFLAG:
			{
				gameData.NextByte();
				uint8_t flag = gameData.ReadByte();
				if(EvaluateCondition() && !stepOver)
				{
					SetFlag(flag, false);
				}
			}
			break;

			case INSTRUCTION_RETURN:
			{
				gameData.NextByte();
				uint16_t value = gameData.ReadWord();
				if (EvaluateCondition() && !stepOver)
				{
					return ExecutionResult(ExecutionResult::Type::Completed, value);
				}
			}
			break;
			
			default:
			return ExecutionResult(ExecutionResult::Type::Completed);
		}
	}
	
	return ExecutionResult(ExecutionResult::Type::Completed);
}

bool VM::EvaluateCondition()
{
	if (gameData.PeekByte() != INSTRUCTION_CONDITION)
	{
		return true;
	}
	gameData.NextByte();

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

VM::ExecutionResult VM::Execute(uint8_t objectIndex, ExecutionMode mode, uint8_t param)
{
	uint16_t dataPointer = GetObjectDataPointer(objectIndex);
	gameData.Seek(dataPointer);
	
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
					bool possible = EvaluateCondition();
					bool stepOver = !possible || mode != ExecutionMode::ExecuteEvent || param != eventId;
					ExecutionResult eventResult = ExecuteInstructions(stepOver);
					
					if(!stepOver)
					{
						return eventResult;
					}
				}
				break;
			case INSTRUCTION_OPTION:
				{
					uint16_t text = gameData.ReadWord();
					bool visible = EvaluateCondition();
					bool stepOver = !visible || mode != ExecutionMode::ExecuteOption || param != optionIndex;
					if (visible)
					{
						if (mode == ExecutionMode::EnumerateOptions)
						{
							optionText[optionIndex] = text;
						}
						optionIndex++;
					}
					ExecutionResult optionResult = ExecuteInstructions(stepOver);
					
					if(!stepOver)
					{
						return optionResult;
					}					
				}
				break;
			default:
			{
				//throw "Unexpected instruction";
			}
			break;
		}
	}
	
	if(mode == ExecutionMode::EnumerateOptions)
	{
		numOptions = optionIndex;
		return ExecutionResult(ExecutionResult::Type::Completed);
	}
	
	return ExecutionResult(ExecutionResult::Type::Failed);
}

void VM::DismissMessage()
{
	if(state == State::ShowingMessage)
	{
		gameData.Seek(lastExecutionResult.returnValue);
		lastExecutionResult = ExecuteInstructions(false);

		if(lastExecutionResult.DidComplete())
		{
			Execute(currentRoomIndex, ExecutionMode::EnumerateOptions);
			state = State::WaitingForOptionChoice;
		}
	}
}

void VM::ChooseOption(uint8_t index)
{
	if(state == State::WaitingForOptionChoice)
	{
		lastExecutionResult = Execute(currentRoomIndex, ExecutionMode::ExecuteOption, index);
	}
}

void VM::UseItem(ItemIndex item)
{
	uint8_t numItems = GetNumGameItems();

	if(state == State::WaitingForOptionChoice)
	{
		if(GetFlag(item) && item < numItems)
		{	
			lastExecutionResult = Execute(currentRoomIndex, ExecutionMode::ExecuteEvent, item);
			if (lastExecutionResult.type == ExecutionResult::Type::Failed)
			{
				lastExecutionResult = Execute(item, ExecutionMode::ExecuteEvent, item);
			}
		}
	}
}

uint8_t VM::GetNumInventoryItems()
{
	uint8_t numItems = GetNumGameItems();
	uint8_t count = 0;

	for (uint8_t n = 0; n < numItems; n++)
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
	uint8_t numItems = GetNumGameItems();
	uint8_t count = 0;

	for (uint8_t n = 0; n < numItems; n++)
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
	uint8_t numItems = GetNumGameItems();

	for (uint8_t n = 0; n < numItems; n++)
	{
		SetFlag(n, true);
	}
}

StringIndex VM::GetItemName(ItemIndex item)
{
	return Execute(item, ExecutionMode::ExecuteEvent, EVENT_NAMEATTRIBUTE).returnValue;
}
StringIndex VM::GetItemDescription(ItemIndex item)
{
	return Execute(item, ExecutionMode::ExecuteEvent, EVENT_DESCRIPTIONATTRIBUTE).returnValue;
}

StringIndex VM::GetCurrentRoomName()
{
	return Execute(currentRoomIndex, ExecutionMode::ExecuteEvent, EVENT_NAMEATTRIBUTE).returnValue;
}
StringIndex VM::GetCurrentRoomDescription()
{
	return Execute(currentRoomIndex, ExecutionMode::ExecuteEvent, EVENT_DESCRIPTIONATTRIBUTE).returnValue;
}

uint8_t VM::GetNumGameItems()
{
	return ReadByteAtAddress(numItemsDataLocation);
}

uint16_t VM::GetObjectDataPointer(uint8_t objectIndex)
{
	return ReadWordAtAddress(objectListDataLocation + objectIndex * 2);
}

uint16_t VM::ReadWordAtAddress(uint16_t address)
{
	uint16_t oldAddress = gameData.Tell();
	gameData.Seek(address);
	uint16_t result = gameData.ReadWord();
	gameData.Seek(oldAddress);
	return result;
}

uint8_t VM::ReadByteAtAddress(uint16_t address)
{
	uint16_t oldAddress = gameData.Tell();
	gameData.Seek(address);
	uint8_t result = gameData.ReadByte();
	gameData.Seek(oldAddress);
	return result;
}

