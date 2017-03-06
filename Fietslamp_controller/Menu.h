class Menu {
private:
	CRGB *leds;

	const uint8_t blinkPeriod = 8;
	uint8_t blinkCounter = 0;
	bool blinkState = true;

	void blink( uint8_t led, uint8_t amount, bool quickly) // max amount is 8 on quickly, 4 on slow. Will keep on repeating then.
	{
		uint8_t currentCount = quickly ? blinkCounter : blinkCounter / 4;

		if(blinkState && currentCount % 2 == 0 && currentCount <= amount) {
			leds[led] = CRGB::White;
		}
	}

public:
	Menu(CRGB *l) { leds = l; }

	void render(uint8_t cMode, uint8_t cSetting)
	{
		//increase blinkCounter
		if (timer100b8 / blinkPeriod % 2 != blinkState) {
			blinkCounter++;
			if(blinkCounter > 8) {
				blinkCounter = 0;
			}
			blinkState != blinkState;
		}

		// Main setting
		switch(cMode) {
			default: //turn off led
				leds[0] = CRGB::Black;
			case 1: // in setting of mode
				blink(0, 2, false);
				break; //double blink
			case 2: // in mode switch
				blink(0, 4, true);
				break;
			case 3: // in brightness change
				blink(0, 8, true);
				break;
		}	

		// Mode setting
		blink(1, cSetting, true); 	
	}
};