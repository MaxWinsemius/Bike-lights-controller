class Menu {
private:
	CRGB *leds;

	bool state[3] = {false, false, false};
	uint8_t counter[2] = {0, 0};
	const uint8_t maxCount[2] = {8, 4};
	const uint8_t speedMultiplier = 1;

	void blink(uint8_t led, uint8_t amount, uint8_t speed) 
	{
		if(state[speed] && counter[speed] < amount) {
			leds[led] = CRGB::White;
		}
	}

	//update states and counter
	void update()
	{
		//set states
		state[0] = (timer300b8 % (2 * speedMultiplier) == 0);
		state[1] = (timer300b8 % (4 * speedMultiplier) == 0);

		//check if changed
		if(state[0] != state[2]) {
			//set changed back
			state[2] = !state[2];

			//increment counter
			for(uint8_t i = 0; i < 2; i++) {
				if(state[i]) {
					counter[i] = counter[i] >= maxCount[i] * speedMultiplier + 1 ? 0 : counter[i] + 1;
				}
			}
		}
	}

public:
	Menu(CRGB *l) { leds = l; }


	void render(uint8_t mode, uint8_t setting)
	{
		Serial.println(setting);
		update();

		// Main setting
		switch(mode) {
			case 1: // in setting of mode
				blink(0, 1, 0);
				break; //double blink
			case 2: // in mode switch
				blink(0, 2, 0);
				break;
			case 3: // in brightness change
				blink(0, 3, 0);
				break;
			default:
				break;
		}	

		// Mode setting
		blink(1, setting, 1); 
	}
};