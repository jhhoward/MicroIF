#include "FrontEnd.h"
#include "Font.h"
#include "Platform.h"

FrontEnd::FrontEnd(VM& inVM)
	: vm(inVM)
{
}

void FrontEnd::DrawStatusBar(const char* message)
{
	Platform::FillRect(0, 0, Platform::DisplayWidth, Font::glyphHeight, 1);
	Font::PrintString(message, 0, 4, true);
	//Platform::FillRect(0, Font::glyphHeight - 1, Platform::DisplayWidth, 1, 0);
}

void FrontEnd::Update()
{
	Platform::FillScreen(0);

	if (vm.GetState() == VM::State::WaitingForOptionChoice)
	{
		constexpr int maxColumns = 3;
		constexpr int columnSpacing = 42;
		uint8_t totalOptions = 0;

		if (showingInventory)
		{
			DrawStatusBar("Inventory");

			uint8_t inventoryCount = vm.GetNumInventoryItems();

			if (inventoryCount > 0)
			{
				int row = 0;
				int column = 0;
				totalOptions = inventoryCount + 1;
				int numRows = ((totalOptions - 1) / maxColumns) + 1;
				int startRow = (Platform::DisplayHeight / Font::glyphHeight) - numRows;

				for (uint8_t slot = 0; slot < inventoryCount; slot++)
				{
					ItemIndex itemIndex = vm.GetInventoryItem(slot);
					StringIndex itemName = vm.GetItemName(itemIndex);
					vm.GetText(itemName, textBuffer);
					Font::PrintString(textBuffer, startRow + row, column * columnSpacing, slot == currentSelection);
					column++;
					if (column == maxColumns)
					{
						column = 0;
						row++;
					}

					if (slot == currentSelection)
					{
						vm.GetText(vm.GetItemDescription(itemIndex), textBuffer);
						Font::PrintString(textBuffer, 1, 0);
					}
				}
				Font::PrintString("back", startRow + row, column * columnSpacing, currentSelection == inventoryCount);

				if (Platform::ButtonDown(Platform::Input::B))
				{
					if (currentSelection < inventoryCount)
					{
						ItemIndex itemIndex = vm.GetInventoryItem(currentSelection);
						vm.UseItem(itemIndex);
					}
					showingInventory = false;
					currentSelection = 0;
				}

				if (Platform::ButtonDown(Platform::Input::A))
				{
					showingInventory = false;
				}
			}
		}
		else
		{
			constexpr uint8_t optionX = 4;

			vm.GetText(vm.GetCurrentRoomName(), textBuffer);
			DrawStatusBar(textBuffer);

			vm.GetText(vm.GetCurrentRoomDescription(), textBuffer);
			Font::PrintString(textBuffer, 1, 0);

			bool showInventoryOption = vm.GetNumInventoryItems() > 0;
			totalOptions = vm.GetNumOptions() + (showInventoryOption ? 1 : 0);

			int row = 0;
			int column = 0;
			int numRows = ((totalOptions - 1) / maxColumns) + 1;
			int startRow = (Platform::DisplayHeight / Font::glyphHeight) - numRows;

			for (uint8_t n = 0; n < vm.GetNumOptions(); n++)
			{
				vm.GetText(vm.GetOptionText(n), textBuffer);
				Font::PrintString(textBuffer, startRow + row, column * columnSpacing, n == currentSelection);
				column++;
				if (column == maxColumns)
				{
					column = 0;
					row++;
				}
			}

			if (showInventoryOption)
			{
				Font::PrintString("items", startRow + row, column * columnSpacing, currentSelection == totalOptions - 1);
			}

			if (Platform::ButtonDown(Platform::Input::B))
			{
				if (showInventoryOption && currentSelection == totalOptions - 1)
				{
					showingInventory = true;
				}
				else
				{
					vm.ChooseOption(currentSelection);
				}
				currentSelection = 0;
			}
		}

		if (Platform::ButtonDown(Platform::Input::Left))
		{
			if (currentSelection == 0)
				currentSelection = totalOptions - 1;
			else
				currentSelection--;
		}
		if (Platform::ButtonDown(Platform::Input::Right))
		{
			currentSelection++;
			if (currentSelection >= totalOptions)
			{
				currentSelection = 0;
			}
		}
		if (Platform::ButtonDown(Platform::Input::Down))
		{
			int currentRow = currentSelection / maxColumns;
			if ((currentRow + 1) * maxColumns < totalOptions)
			{
				currentSelection += 3;
				if (currentSelection >= totalOptions)
				{
					currentSelection = totalOptions - 1;
				}
			}
		}
		if (Platform::ButtonDown(Platform::Input::Up))
		{
			if (currentSelection - 3 >= 0)
			{
				currentSelection -= 3;
			}
		}
	}
	else if (vm.GetState() == VM::State::ShowingMessage)
	{
		vm.GetText(vm.GetCurrentRoomName(), textBuffer);
		DrawStatusBar(textBuffer);
	
		vm.GetText(vm.GetCurrentMessageText(), textBuffer);

		const char* message = textBuffer + messageOffset;
		int toWrite = messageCharactersToWrite - messageOffset;
		int charactersWritten = Font::PrintString(message, 1, 0, false, toWrite);

		if (charactersWritten == toWrite)
		{
			messageCharactersToWrite++;

			if (Platform::ButtonDown(Platform::Input::B))
			{
				charactersWritten = Font::PrintString(message, 1, 0, false);
				messageCharactersToWrite = messageOffset + charactersWritten;
			}
		}
		else
		{
			bool screenClipped = message[charactersWritten] != '\0';

			if (screenClipped)
			{
				if (Platform::ButtonDown(Platform::Input::B))
				{
					messageOffset += charactersWritten;
				}
			}
			else
			{
				if (Platform::ButtonDown(Platform::Input::B))
				{
					vm.DismissMessage();
					messageOffset = 0;
					messageCharactersToWrite = 0;
				}
			}
		}
	}

	//Font::PrintString("Inverted text", 1, 0, true);
}
