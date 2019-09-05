#pragma once
#include <string>
#include "Game.h"
#include "../IF/InstructionCodes.h"
#include "StringTable.h"

class Instruction
{
public:
	virtual void Emit(Game& game, GameDataWriter& writer) = 0;
};

class BasicInstruction : public Instruction
{
public:
	BasicInstruction(uint8_t inInstruction, const std::string& inComment) : instruction(inInstruction), comment(inComment) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		writer.Write(instruction, comment);
	}	

private:
	uint8_t instruction;
	std::string comment;
};

class ReturnInstruction : public Instruction
{
public:
	ReturnInstruction(std::string& inReturnValue) : returnValue(inReturnValue) {}

	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		writer.Write((uint8_t)INSTRUCTION_RETURN, game.GetStringTable().GetIndex(returnValue), "\"" + returnValue + "\"");
	}

private:
	std::string returnValue;
};

class EventInstruction : public Instruction
{
public:
	EventInstruction(uint8_t inEvent, const std::string& inComment) : event(inEvent), comment(inComment) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		writer.Write((uint8_t) INSTRUCTION_EVENT, event, comment);
	}	

private:
	uint8_t event;
	std::string comment;
};

class OptionInstruction : public Instruction
{
public:
	OptionInstruction(const std::string& inText) : text(inText) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{		
		writer.Write((uint8_t) INSTRUCTION_OPTION, game.GetStringTable().GetIndex(text), "option \"" + text + "\"");
	}

protected:	
	std::string text;
};

class GoInstruction : public Instruction
{
public:
	GoInstruction(const std::string& inLocation) : location(inLocation) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		int locationIndex = game.GetRoom(location)->index;
		writer.Write((uint8_t) INSTRUCTION_GO, (uint8_t) locationIndex, "go " + location);
	}

protected:	
	std::string location;
};

class SayInstruction : public Instruction
{
public:
	SayInstruction(const std::string& inMessage) : message(inMessage) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		writer.Write((uint8_t) INSTRUCTION_SAY, game.GetStringTable().GetIndex(message), "say \"" + message + "\"");
	}

protected:	
	std::string message;
};

class EvalFlagInstruction : public Instruction
{
public:
	EvalFlagInstruction(const std::string& inFlag) : flag(inFlag) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		int flagIndex = game.GetFlagIndex(flag);
		writer.Write((uint8_t) INSTRUCTION_EVALUATEFLAG, (uint8_t) flagIndex, "eval " + flag);
	}

protected:	
	std::string flag;
};

class EvalHaveItemInstruction : public Instruction
{
public:
	EvalHaveItemInstruction(const std::string& inItem) : item(inItem) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		int flagIndex = game.GetItem(item)->index;
		writer.Write((uint8_t) INSTRUCTION_EVALUATEFLAG, (uint8_t) flagIndex, "have " + item);
	}

protected:	
	std::string item;
};

class UseItemInstruction : public Instruction
{
public:
	UseItemInstruction(const std::string& inItem) : item(inItem) {}

	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		int flagIndex = game.GetItem(item)->index;
		writer.Write((uint8_t)INSTRUCTION_EVENT, (uint8_t)flagIndex, "use " + item);
	}

protected:
	std::string item;
};

class SetItemStateInstruction : public Instruction
{
public:
	SetItemStateInstruction(const std::string& inItem, bool inPossessed) : item(inItem), possessed(inPossessed) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		int itemIndex = game.GetItem(item)->index;

		if(possessed)
		{
			writer.Write((uint8_t) INSTRUCTION_SETFLAG, (uint8_t) itemIndex, "give " + item);	
		}
		else
		{
			writer.Write((uint8_t) INSTRUCTION_UNSETFLAG, (uint8_t) itemIndex, "remove " + item);	
		}
	}

protected:	
	std::string item;
	bool possessed;
};

class SetFlagInstruction : public Instruction
{
public:
	SetFlagInstruction(const std::string& inFlag, bool inValue) : flag(inFlag), value(inValue) {}
	
	virtual void Emit(Game& game, GameDataWriter& writer) override
	{
		int flagIndex = game.GetFlagIndex(flag);

		if(value)
		{
			writer.Write((uint8_t) INSTRUCTION_SETFLAG, (uint8_t) flagIndex, "set " + flag);	
		}
		else
		{
			writer.Write((uint8_t) INSTRUCTION_UNSETFLAG, (uint8_t) flagIndex, "unset " + flag);	
		}
	}

protected:	
	std::string flag;
	bool value;
};
