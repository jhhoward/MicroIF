#pragma once

#include <stdint.h>

class Font
{
public:
	static constexpr int glyphWidth = 4;
	static constexpr int glyphHeight = 8;
	static constexpr int firstGlyphIndex = 32;

	static int PrintString(const char* str, uint8_t line, uint8_t x, bool invert = false, int maxCharacters = -1);
	static void PrintInt(uint16_t value, uint8_t line, uint8_t x, bool invert = false);

private:
	static void DrawChar(uint8_t* screenPtr, char c, uint8_t xorMask);
};
