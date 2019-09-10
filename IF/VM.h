#pragma once
#include <stdint.h>

typedef uint16_t StringIndex;
typedef uint8_t ItemIndex;

class DataArchive
{
public:
	virtual void Seek(uint16_t seekPos) = 0;
	virtual uint16_t Tell() = 0;
	virtual uint8_t PeekByte() = 0;
	virtual uint8_t ReadByte() = 0;
	virtual uint16_t ReadWord() = 0;
	virtual void NextByte() = 0;
};

enum class ExecutionMode
{
	EnumerateOptions,
	ExecuteOption,
	ExecuteEvent,
};

class VM
{
public:
	enum State
	{
		ShowingMessage,
		WaitingForOptionChoice,
		Executing
	};

	VM(DataArchive& inGameData, DataArchive& stringData);
	
	// State management
	void Reset();
	State GetState() { return state; }
	StringIndex GetCurrentRoomName();
	StringIndex GetCurrentRoomDescription();

	// Item management
	uint8_t GetNumInventoryItems();
	ItemIndex GetInventoryItem(uint8_t slotIndex);
	StringIndex GetItemName(ItemIndex item);
	StringIndex GetItemDescription(ItemIndex item);	
	void UseItem(ItemIndex item);
	void UnlockAllItems();

	// Options
	uint8_t GetNumOptions()	{ return numOptions; }
	StringIndex GetOptionText(uint8_t index) { return optionText[index]; }
	void ChooseOption(uint8_t index);

	StringIndex GetCurrentMessageText() { return currentMessageText; }
	void DismissMessage();
	
	void GetText(StringIndex stringIndex, char* buffer);
	
private:
	struct ExecutionResult
	{
		enum class Type
		{
			Completed = 0,
			Failed = 1,
			Yielded = 2
		};

		ExecutionResult() {}
		ExecutionResult(Type inType, uint16_t inReturnValue = 0xffff) : type(inType), returnValue(inReturnValue) {}

		Type type;
		uint16_t returnValue = 0xffff;

		bool DidYield() { return type == Type::Yielded; }
		bool DidComplete() { return type == Type::Completed; }
	};

	ExecutionResult Execute(uint8_t objectIndex, ExecutionMode mode, uint8_t param = 0);
	ExecutionResult ExecuteInstructions(bool stepOver);

	ExecutionResult EnterRoom(uint8_t newRoom);

	bool GetFlag(uint8_t index);
	void SetFlag(uint8_t index, bool value);
	bool EvaluateCondition();

	void GetTextInternal(StringIndex stringIndex, char** buffer);
	void GetCodedPhrase(uint8_t index, char** buffer);

	uint8_t GetNumGameItems();
	uint16_t GetObjectDataPointer(uint8_t objectIndex);

	uint16_t ReadWordAtAddress(uint16_t address);
	uint8_t ReadByteAtAddress(uint16_t address);

	State state;
	ExecutionResult lastExecutionResult;

	static constexpr uint16_t flagDefaultDataLocation = 0;
	static constexpr uint16_t numObjectsDataLocation = 32;
	static constexpr uint16_t numItemsDataLocation = 33;
	static constexpr uint16_t startingRoomDataLocation = 34;
	static constexpr uint16_t objectListDataLocation = 35;

	static constexpr int maxOptions = 10;
	StringIndex optionText[maxOptions];
	uint8_t numOptions;
	StringIndex currentMessageText;

	DataArchive& stringData;
	DataArchive& gameData;
	
	uint8_t currentRoomIndex;
	uint8_t flagState[256 / 8];
};
