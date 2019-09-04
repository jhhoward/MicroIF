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

struct RoomHeader
{
	StringIndex title;
	StringIndex description;
	uint16_t dataPointer;
};

struct ItemHeader
{
	StringIndex title;
	StringIndex description;
};

struct DataHeader
{
	StringIndex numItems;
	StringIndex numRooms;
};

enum class ExecutionMode
{
	EnumerateOptions,
	ExecuteOption,
	ExecuteItemOption,
	ExecuteEvent
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
	RoomHeader GetCurrentRoomHeader()	{ return GetRoomHeader(currentRoomIndex); }
	void LookAround();

	// Item management
	uint8_t GetNumInventoryItems();
	ItemIndex GetInventoryItem(uint8_t slotIndex);
	ItemHeader GetItemHeader(ItemIndex item);
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
	bool Execute(ExecutionMode mode, uint8_t param = 0);
	bool GetFlag(uint8_t index);
	void SetFlag(uint8_t index, bool value);
	bool EvaluateCondition();
	bool ExecuteInstructions(bool stepOver);

	DataHeader GetDataHeader();
	RoomHeader GetRoomHeader(uint8_t roomIndex);

	void GetTextInternal(StringIndex stringIndex, char** buffer);
	void GetCodedPhrase(uint8_t index, char** buffer);

	State state;

	static constexpr int maxOptions = 10;
	StringIndex optionText[maxOptions];
	uint8_t numOptions;
	StringIndex currentMessageText;

	DataArchive& stringData;
	DataArchive& gameData;
	
	uint8_t currentRoomIndex;
	uint8_t flagState[256 / 8];
};
