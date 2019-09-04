#include <Windows.h>
#include <SDL.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include "Platform.h"
#include "Font.h"
#include "FrontEnd.h"
#include "lodepng.h"

#define ZOOM_SCALE 1
#define TONES_END 0x8000

SDL_Window* AppWindow;
SDL_Renderer* AppRenderer;
SDL_Surface* ScreenSurface;
SDL_Texture* ScreenTexture;

uint8_t lastInputMask = 0;
uint8_t currentInputMask = 0;
uint8_t sBuffer[Platform::DisplayWidth * Platform::DisplayHeight / 8];

bool isAudioEnabled = true;
bool IsRecording = false;
int CurrentRecordingFrame = 0;

struct KeyMap
{
	SDL_Scancode key;
	Platform::Input mask;
};

std::vector<KeyMap> KeyMappings =
{
	{ SDL_SCANCODE_LEFT, Platform::Input::Left },
	{ SDL_SCANCODE_RIGHT, Platform::Input::Right },
	{ SDL_SCANCODE_UP, Platform::Input::Up },
	{ SDL_SCANCODE_DOWN, Platform::Input::Down },
	{ SDL_SCANCODE_Z, Platform::Input::A },
	{ SDL_SCANCODE_X, Platform::Input::B },
};

constexpr int audioSampleRate = 48000;

const uint16_t* currentAudioPattern = nullptr;
int currentPatternBufferPos = 0;

void Play(const uint16_t* pattern)
{
	currentAudioPattern = pattern;
	currentPatternBufferPos = 0;
}

void swap(int16_t& a, int16_t& b)
{
	int16_t temp = a;
	a = b;
	b = temp;
}

void FillAudioBuffer(void *udata, uint8_t *stream, int len)
{
	int feedPos = 0;

	static int waveSamplesLeft = 0;
	static int noteSamplesLeft = 0;
	static int frequency = 0;
	static bool high = false;

	while (feedPos < len)
	{
		if (!isAudioEnabled)
		{
			while (feedPos < len)
			{
				stream[feedPos++] = 0;
			}
			return;
		}

		if (currentAudioPattern != nullptr)
		{
			if (noteSamplesLeft == 0)
			{
				frequency = currentAudioPattern[currentPatternBufferPos];
				uint16_t duration = currentAudioPattern[currentPatternBufferPos + 1];

				noteSamplesLeft = (audioSampleRate * duration) / 1000;

				waveSamplesLeft = frequency > 0 ? audioSampleRate / frequency : noteSamplesLeft;

				currentPatternBufferPos += 2;
				if (currentAudioPattern[currentPatternBufferPos] == TONES_END)
				{
					currentAudioPattern = nullptr;
				}
			}
		}

		if (frequency == 0)
		{
			while (feedPos < len && (!currentAudioPattern || noteSamplesLeft > 0))
			{
				stream[feedPos++] = 0;

				if (noteSamplesLeft > 0)
					noteSamplesLeft--;
			}
		}
		else
		{
			while (feedPos < len && waveSamplesLeft > 0 && noteSamplesLeft > 0)
			{
				int volume = 32;
				stream[feedPos++] = high ? 128 + volume : 128 - volume;
				waveSamplesLeft--;
				noteSamplesLeft--;
			}

			if (waveSamplesLeft == 0)
			{
				high = !high;
				waveSamplesLeft = audioSampleRate / frequency;
			}
		}

	}
}

void Platform::SetLED(uint8_t r, uint8_t g, uint8_t b)
{

}

void Platform::FillScreen(uint8_t colour)
{
	for (int y = 0; y < DisplayHeight; y++)
	{
		for (int x = 0; x < DisplayWidth; x++)
		{
			PutPixel(x, y, colour);
		}
	}
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
	uint8_t w = bitmap[0];
	uint8_t h = bitmap[1];

	bitmap += 2;

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			int blockY = j / 8;
			int blockIndex = (w * blockY + i) * 2;
			uint8_t pixels = bitmap[blockIndex];
			uint8_t maskPixels = bitmap[blockIndex + 1];
			uint8_t bitmask = 1 << (j % 8);

			if (maskPixels & bitmask)
			{
				if (x + i >= 0 && y + j >= 0)
				{
					if (pixels & bitmask)
					{
						PutPixel(x + i, y + j, 1);
					}
					else
					{
						PutPixel(x + i, y + j, 0);
					}
				}
			}
		}
	}
}

void Platform::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
	if (x >= DisplayWidth || y >= DisplayHeight)
	{
		return;
	}

	uint16_t row_offset;
	uint8_t bit;

	bit = 1 << (y & 7);
	row_offset = (y & 0xF8) * DisplayWidth / 8 + x;
	uint8_t data = sBuffer[row_offset] | bit;
	if (!colour) data ^= bit;
	sBuffer[row_offset] = data;
}

uint8_t Platform::GetPixel(uint8_t x, uint8_t y)
{
	uint8_t row = y / 8;
	uint8_t bit_position = y % 8;
	return (sBuffer[(row*Platform::DisplayWidth) + x] & (1 << bit_position)) >> bit_position;
}

uint8_t* Platform::GetScreenBuffer()
{
	return sBuffer;
}

void ResolveScreen(SDL_Surface* surface)
{
	Uint32 black = SDL_MapRGBA(surface->format, 0, 0, 0, 255);
	Uint32 white = SDL_MapRGBA(surface->format, 255, 255, 255, 255);

	int bpp = surface->format->BytesPerPixel;

	for (int y = 0; y < Platform::DisplayHeight; y++)
	{
		for (int x = 0; x < Platform::DisplayWidth; x++)
		{
			Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

			*(Uint32 *)p = Platform::GetPixel(x, y) ? white : black;
		}
	}
}

void Platform::PlaySound(const uint16_t* audioPattern)
{
	Play(audioPattern);
}

bool Platform::IsAudioEnabled()
{
	return isAudioEnabled;
}

void Platform::SetAudioEnabled(bool isEnabled)
{
	isAudioEnabled = isEnabled;
}

bool Platform::ButtonDown(Input maskEnum)
{
	uint8_t mask = (uint8_t)maskEnum;

	return (currentInputMask & mask) != 0 && (lastInputMask & mask) == 0;
}

void Platform::FillRect(int16_t x1, int16_t y1, uint8_t w, uint8_t h, uint8_t colour)
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			PutPixel((uint8_t)x1 + x, (uint8_t)y1 + y, colour);
		}
	}
}

/*#include "Font.h"
void DumpFont()
{
	constexpr int numChars = 96;
	constexpr int charWidth = 4;
	constexpr int charHeight = 8;
	constexpr int pageWidth = numChars * charWidth;
	uint8_t fontPage[pageWidth * charHeight * 4];
	char tempStr[2] = { 0, 0 };

	for (int n = 0; n < numChars * charWidth * charHeight * 4; n++)
	{
		fontPage[n] = 255;
	}

	for (int n = 0; n < numChars; n++)
	{
		Platform::FillScreen(COLOUR_WHITE);
		tempStr[0] = (char) (n + 32);
		DrawString(tempStr, 0, 1);

		for (int y = 0; y < charHeight; y++)
		{
			for (int x = 0; x < charWidth; x++)
			{
				if (GetPixel(x, y) == COLOUR_BLACK)
				{
					int index = pageWidth * y + (n * charWidth) + x;
					fontPage[index * 4] = 0;
					fontPage[index * 4 + 1] = 0;
					fontPage[index * 4 + 2] = 0;
				}
			}
		}
	}

	lodepng::encode("font.png", fontPage, pageWidth, charHeight);
}*/

uint8_t GetInputMask()
{
	uint8_t inputMask = 0;

	const uint8_t* keyStates = SDL_GetKeyboardState(NULL);

	for (unsigned int n = 0; n < KeyMappings.size(); n++)
	{
		if (keyStates[KeyMappings[n].key])
		{
			inputMask |= (uint8_t) KeyMappings[n].mask;
		}
	}

	return inputMask;
}

class SimpleDataArchive : public DataArchive
{
public:
	SimpleDataArchive(const uint8_t* inBasePtr) : basePtr(inBasePtr), offset(0) {}

	virtual void Seek(uint16_t seekPos) override { offset = seekPos; }
	virtual uint8_t PeekByte() override
	{
		return basePtr[offset];
	}
	virtual uint8_t ReadByte() override
	{
		return basePtr[offset++];
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

const uint8_t gameDataBytes[] =
{
	#include "Generated/GameData.inc.h"
};

const uint8_t stringDataBytes[] =
{
	#include "Generated/StringData.inc.h"
};

SimpleDataArchive gameData(gameDataBytes);
SimpleDataArchive stringData(stringDataBytes);
VM vm(gameData, stringData);
FrontEnd frontEnd(vm);

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_CreateWindowAndRenderer(Platform::DisplayWidth * ZOOM_SCALE, Platform::DisplayHeight * ZOOM_SCALE, SDL_WINDOW_RESIZABLE, &AppWindow, &AppRenderer);
	SDL_RenderSetLogicalSize(AppRenderer, Platform::DisplayWidth, Platform::DisplayHeight);

	ScreenSurface = SDL_CreateRGBSurface(0, Platform::DisplayWidth, Platform::DisplayHeight, 32,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	);
	ScreenTexture = SDL_CreateTexture(AppRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, ScreenSurface->w, ScreenSurface->h);

	//SDL_SetWindowPosition(AppWindow, 1900 - DISPLAY_WIDTH * 2, 1020 - DISPLAY_HEIGHT);

	SDL_AudioSpec wanted;
	wanted.freq = audioSampleRate;
	wanted.format = AUDIO_U8;
	wanted.channels = 1;
	wanted.samples = 4096;
	wanted.callback = FillAudioBuffer;

	if (SDL_OpenAudio(&wanted, NULL) < 0) {
		printf("Error: %s\n", SDL_GetError());
	}
	SDL_PauseAudio(0);

	//Game::Init();

	bool running = true;
	int playRate = 1;
	static int testAudio = 0;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_F12:
				{
					lodepng::encode(std::string("screenshot.png"), (unsigned char*)(ScreenSurface->pixels), ScreenSurface->w, ScreenSurface->h);
				}
				break;
				case SDLK_F11:
					IsRecording = !IsRecording;
					break;
				}
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_TAB)
					playRate = 1;
				break;
			}
		}

		SDL_SetRenderDrawColor(AppRenderer, 206, 221, 231, 255);
		SDL_RenderClear(AppRenderer);

		for (int n = 0; n < playRate; n++)
		{
			memset(ScreenSurface->pixels, 0, ScreenSurface->format->BytesPerPixel * ScreenSurface->w * ScreenSurface->h);

			//Game::Tick();
			//Game::Draw();
			frontEnd.Update();

			ResolveScreen(ScreenSurface);

			lastInputMask = currentInputMask;
			currentInputMask = GetInputMask();
		}

		if (IsRecording)
		{
			std::ostringstream filename;
			filename << "Frame";
			filename << std::setfill('0') << std::setw(5) << CurrentRecordingFrame << ".png";

			lodepng::encode(filename.str(), (unsigned char*)(ScreenSurface->pixels), ScreenSurface->w, ScreenSurface->h);
			CurrentRecordingFrame++;
		}

		SDL_UpdateTexture(ScreenTexture, NULL, ScreenSurface->pixels, ScreenSurface->pitch);
		SDL_Rect src, dest;
		src.x = src.y = dest.x = dest.y = 0;
		src.w = Platform::DisplayWidth;
		src.h = Platform::DisplayHeight;
		dest.w = Platform::DisplayWidth;
		dest.h = Platform::DisplayHeight;
		SDL_RenderCopy(AppRenderer, ScreenTexture, &src, &dest);
		SDL_RenderPresent(AppRenderer);

		SDL_Delay(1000 / Platform::TargetFrameRate);
	}

	return 0;
}
