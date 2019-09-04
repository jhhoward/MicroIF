#include <Arduboy2.h>
#include <avr/pgmspace.h>
#include "FrontEnd.h"
#include "VM.h"
#include "Platform.h"

class PGMDataArchive : public DataArchive
{
public:
	PGMDataArchive(const uint8_t* inBasePtr) : basePtr(inBasePtr), offset(0) {}

	virtual void Seek(uint16_t seekPos) override { offset = seekPos; }
	virtual uint8_t PeekByte() override
	{
		return pgm_read_byte(&basePtr[offset]);
	}
	virtual uint8_t ReadByte() override
	{
		return pgm_read_byte(&basePtr[offset++]);
	}
	virtual uint16_t ReadWord() override
	{
		return ReadByte() | (ReadByte() << 8);
	}
	virtual void NextByte() override
	{
		offset++;
	}
	virtual uint16_t Tell()
	{
		return offset;
	}

private:
	const uint8_t* basePtr;
	uint16_t offset;
};

const uint8_t gameDataBytes[] PROGMEM =
{
	#include "Generated/GameData.inc.h"
};

const uint8_t stringDataBytes[] PROGMEM =
{
	#include "Generated/StringData.inc.h"
};

Arduboy2Base arduboy;
PGMDataArchive gameData(gameDataBytes);
PGMDataArchive stringData(stringDataBytes);
VM vm(gameData, stringData);
FrontEnd frontEnd(vm);

void Platform::PlaySound(const uint16_t* audioPattern)
{
	//sound.tones(audioPattern);
}

void Platform::SetLED(uint8_t r, uint8_t g, uint8_t b)
{
  arduboy.digitalWriteRGB(r ? RGB_ON : RGB_OFF, g ? RGB_ON : RGB_OFF, b ? RGB_ON : RGB_OFF);
}

void Platform::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
  arduboy.drawPixel(x, y, colour);
}

uint8_t* Platform::GetScreenBuffer()
{
  return arduboy.getBuffer();
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
//  sprites.drawPlusMask(x, y, bitmap, frame);
}

void Platform::FillScreen(uint8_t colour)
{
  arduboy.fillScreen(colour);
}

bool Platform::IsAudioEnabled()
{
	return arduboy.audio.enabled();
}

void Platform::SetAudioEnabled(bool isEnabled)
{
	if(isEnabled)
		arduboy.audio.on();
	else
		arduboy.audio.off();
}

bool Platform::ButtonDown(Platform::Input input)
{
	switch(input)
	{
		case Input::Left:
			return arduboy.justPressed(LEFT_BUTTON);
		case Input::Right:
			return arduboy.justPressed(RIGHT_BUTTON);
		case Input::Up:
			return arduboy.justPressed(UP_BUTTON);
		case Input::Down:
			return arduboy.justPressed(DOWN_BUTTON);
		case Input::A:
			return arduboy.justPressed(A_BUTTON);
		case Input::B:
			return arduboy.justPressed(B_BUTTON);
	}
	return false;
}

void Platform::FillRect(int16_t x1, int16_t y1, uint8_t w, uint8_t h, uint8_t colour)
{
	arduboy.fillRect(x1, y1, w, h, colour);
}

void setup()
{
  arduboy.boot();
  arduboy.flashlight();
  arduboy.systemButtons();
  //arduboy.bootLogo();
  arduboy.setFrameRate(Platform::TargetFrameRate);

  //arduboy.audio.off();
  
  //Serial.begin(9600);

//  SeedRandom((uint16_t) arduboy.generateRandomSeed());
}

void loop()
{
  if(arduboy.nextFrame())
  {
    frontEnd.Update();
    //Serial.write(arduboy.getBuffer(), 128 * 64 / 8);

	
    arduboy.display(false);
	arduboy.pollButtons();
  }
}
