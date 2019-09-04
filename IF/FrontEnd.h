#pragma once

#include "VM.h"

class FrontEnd
{
public:
	FrontEnd(VM& inVM);
	void Update();

private:
	void DrawStatusBar(const char* message);

	char textBuffer[512];

	VM vm;

	uint8_t currentSelection = 0;
	int messageCharactersToWrite = 0;
	int messageOffset = 0;
	bool showingInventory = false;
};
