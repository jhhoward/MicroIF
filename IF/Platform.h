#pragma once

#include <stdint.h>

#if _WIN32
#define PROGMEM
#define pgm_read_byte(x) (*((uint8_t*)x))
#define pgm_read_word(x) (*((uint16_t*)x))
#define pgm_read_ptr(x) (*((uintptr_t*)x))
#else
#include <avr/pgmspace.h>
#endif

class Platform
{
public:
	enum class Input
	{
		Left = 1,
		Right = 2,
		Up = 4,
		Down = 8,
		A = 16,
		B = 32
	};

	static constexpr int DisplayWidth = 128;
	static constexpr int DisplayHeight = 64;
	static constexpr int TargetFrameRate = 60;

	static void SetLED(uint8_t r, uint8_t g, uint8_t b);

	static void FillScreen(uint8_t colour);
	static void PutPixel(uint8_t x, uint8_t y, uint8_t colour);
	static uint8_t GetPixel(uint8_t x, uint8_t y);
	static uint8_t* GetScreenBuffer();
	static void DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame);
	static void FillRect(int16_t x1, int16_t y1, uint8_t w, uint8_t h, uint8_t colour);

	static void PlaySound(const uint16_t* audioPattern);
	static bool IsAudioEnabled();
	static void SetAudioEnabled(bool isEnabled);

	static bool ButtonDown(Input mask);
};
