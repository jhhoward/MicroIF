class VMInterface
{
	virtual void OnShowMessage(StringIndex message) = 0;
};

class FrontEnd : public VMInterface
{
public:
	void Update();
	
private:
	enum class State
	{
		Message,
		Options,
		Items
	};

	void Print(const char* message, uint8_t x, uint8_t y, uint8_t colour);

	void DrawTitle();
	void UpdateMessage();
	
	virtual void OnShowMessage(StringIndex message) override;

	State currentState;

	// For Message state
	uint8_t messageCharactersShown;
	StringIndex currentMessage;
	int currentMessageX, currentMessageY;
	bool isTypingMessage;

	uint8_t selection;

	char textBuffer[256];
};


void FrontEnd::Print(const char* message, uint8_t x, uint8_t y, uint8_t colour)
{
	while(*message)
	{
		PrintChar(*message, x, y, colour);
		x++;
		message++;
	}
}

void FrontEnd::DrawTitle()
{
	RoomHeader header = vm.GetCurrentRoomHeader();
	vm.GetString(header.title, textBuffer);
	Print(textBuffer, 0, 0, COLOUR_BLACK);
}

void FrontEnd::OnShowMessage(StringIndex message)
{
	currentMessage = message;
	isTypingMessage = true;
	messageCharactersShown = 0;
	currentMessageX = 0;
	currentMessageY = 1;
	ClearTextArea();
}

void FrontEnd::UpdateMessage()
{
	if(isTypingMessage)
	{
		messageCharactersShown++;
	}
	
	vm.GetString(currentMessage, textBuffer);
	
}

void FrontEnd::Update()
{
	switch(currentState)
	{
		case Message:
		
		break;
	}
}
