#pragma once
#include <stdint.h>

class ScriptPointer
{
public:
	ScriptPointer() {}
	ScriptPointer(const uint8_t* inBasePtr) : basePtr(inBasePtr), offset(0) {}
	ScriptPointer(const uint8_t* inBasePtr, uint16_t inOffset) : basePtr(inBasePtr), offset(inOffset) {}
	
	void Seek(uint16_t seekPos) { offset = seekPos; }
	uint8_t PeekByte()
	{
		return basePtr[offset];
	}
	uint8_t ReadByte()
	{
		return basePtr[offset++];
	}
	uint16_t ReadWord()
	{
		return ReadByte() | (ReadByte() << 8);
	}
	void NextByte()
	{
		offset++;
	}
	
private:
	const uint8_t* basePtr;
	uint16_t offset;
};

struct RoomHeader
{
	int16_t title;
	int16_t description;
	ScriptPointer dataPointer;
};

struct ItemHeader
{
	int16_t title;
	int16_t description;
};

struct DataHeader
{
	uint8_t numItems;
	uint8_t numRooms;
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
	VM(const uint8_t* inGameData);
	
	void Reset();
	void PrintOptions();
	void ChooseOption(uint8_t index);
	void UseItem(uint8_t index);
	
	uint8_t GetCurrentRoomIndex() { return currentRoomIndex; }
	
	RoomHeader GetRoomHeader(uint8_t roomIndex);
	ItemHeader GetItemHeader(uint8_t itemIndex);
	DataHeader GetDataHeader();
	
	int GetNumOptions() { return numOptions; }
	
private:
	bool Execute(ExecutionMode mode, uint8_t param = 0);
	bool GetFlag(uint8_t index);
	void SetFlag(uint8_t index, bool value);
	bool EvaluateCondition(ScriptPointer& ptr);
	void ExecuteOption(ScriptPointer& ptr, bool stepOver);


	static constexpr int maxOptions = 10;
	int16_t optionText[maxOptions];
	uint8_t numOptions;

	const uint8_t* gameData;
	
	uint8_t currentRoomIndex;
	uint8_t flagState[256 / 8];
};
