#include <stdint.h>
#include "Font.h"
#include "Platform.h"
#include "Generated/SpriteTypes.h"
#include "Generated/SpriteData.inc.h"

int Font::PrintString(const char* message, uint8_t line, uint8_t x, bool invert, int maxCharacters)
{
	const char* str = message;
	uint8_t* screenPtr = Platform::GetScreenBuffer();
	uint8_t* screenEnd = screenPtr + (Platform::DisplayWidth * Platform::DisplayHeight / 8);
	uint8_t xorMask = invert ? 0 : 0xff;
	uint8_t* lineStart = screenPtr + Platform::DisplayWidth * line;
	screenPtr = lineStart + x;
	int charactersWritten = 0;

	for (;;)
	{
		if (maxCharacters >= 0 && charactersWritten >= maxCharacters)
			break;

		char c = *str;
		str++;
		if (!c)
			break;

		if (c == '\n')
		{
			lineStart += Platform::DisplayWidth;
			screenPtr = lineStart + x;
		}
		else
		{
			DrawChar(screenPtr, c, xorMask);
			screenPtr += glyphWidth;

			if (c == ' ')
			{
				uint8_t wordLength = 0;
				// Check if we need to insert a line break
				for (const char* ptr = str; *ptr; ptr++)
				{
					if (*ptr == ' ')
					{
						break;
					}
					wordLength++;
				}
				if (screenPtr + glyphWidth * wordLength >= lineStart + Platform::DisplayWidth)
				{
					lineStart += Platform::DisplayWidth;
					screenPtr = lineStart + x;
				}
			}
			if (screenPtr >= lineStart + Platform::DisplayWidth)
			{
				lineStart += Platform::DisplayWidth;
				screenPtr = lineStart + x;
			}
		}

		charactersWritten++;

		if (screenPtr >= screenEnd)
		{
			break;
		}
	}

	return charactersWritten;
}

void Font::PrintInt(uint16_t val, uint8_t line, uint8_t x, bool invert)
{
	uint8_t* screenPtr = Platform::GetScreenBuffer();
	uint8_t xorMask = invert ? 0 : 0xff;
	uint8_t* lineStart = screenPtr + Platform::DisplayWidth * line + x;
	screenPtr = lineStart;

	if (val == 0)
	{
		DrawChar(screenPtr, '0', xorMask);
		return;
	}

	constexpr int maxDigits = 5;
	char buffer[maxDigits];
	int bufCount = 0;

	for (int n = 0; n < maxDigits && val != 0; n++)
	{
		unsigned char c = val % 10;
		buffer[bufCount++] = '0' + c;
		val = val / 10;
	}

	for (int n = bufCount - 1; n >= 0; n--)
	{
		DrawChar(screenPtr, buffer[n], xorMask);
		screenPtr += glyphWidth;
	}

}

void Font::DrawChar(uint8_t* screenPtr, char c, uint8_t xorMask)
{
	const uint8_t index = ((unsigned char)(c)) - firstGlyphIndex;
	const uint8_t* fontPtr = fontPageData + glyphWidth * index;

	screenPtr[0] = xorMask ^ pgm_read_byte(&fontPtr[0]);
	screenPtr[1] = xorMask ^ pgm_read_byte(&fontPtr[1]);
	screenPtr[2] = xorMask ^ pgm_read_byte(&fontPtr[2]);
	screenPtr[3] = xorMask ^ pgm_read_byte(&fontPtr[3]);

//	screenPtr[0] = xorMask ^ pgm_read_byte(&fontPtr[0]);
//	screenPtr[1] = xorMask ^ pgm_read_byte(&fontPtr[1]);
//	screenPtr[2] = xorMask ^ pgm_read_byte(&fontPtr[2]);
//	screenPtr[3] = xorMask ^ pgm_read_byte(&fontPtr[3]);
}
