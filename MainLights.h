class MainLights: public Mode {
private:
	const uint8_t indicatorPeriod = 10;
	bool indToggle[2] = {false, false}; //indicator toggle; left, right
	bool indActivated = false; //indicator runner started (used to check if next indicator round has started)
	uint8_t indRunLight = 0; //indicator running light index
	bool indSwitch = false; //Timer to see when to restart

public:
	MainLights (CRGB *l) : Mode(l) {}

	void render()
	{
		//Main front light
		uint8_t h = calcSine(timer20b8, 149, 74, 0), 
				s = calcSine(timer300b8, 200, 0, 0), 
				v = calcSine(timer20b8, 255, 198, 0);
		CHSV clr = CHSV(h, s, v);

		//Fill front light part
		uint8_t startLed = !indToggle[0]&&!indToggle[1] ? sideBars[0][0] : sideBars[0][1];
		uint8_t endLed = (!indToggle[0]&&!indToggle[1] ? sideBars[1][1] : sideBars[1][0]) - startLed;
		fill_solid(&leds[startLed], endLed, clr);

		//indicators. note; overrides main lights
		if(indToggle[0] || indToggle[1]) {
			//clear prev led val in blinker areas
			uint8_t scale = 240;
			fadeLeds(sideBars[0][0], sideBars[0][1], scale);
			fadeLeds(sideBars[1][0], sideBars[1][1], scale);

			//roll lights
			for (int i = 0; i < 2; ++i)
			{
				if(indToggle[i]) {
					uint8_t starterLed = (i == 0 ? sideBars[0][1] : sideBars[1][0]); //Get first led of bar
					uint8_t currentLed = (i == 0 ? VarHandler::capUint8(starterLed,-(int16_t)indRunLight) : starterLed + indRunLight); //Calculate what led would turn on based on starter led
					if(i == 0) {
						if(!VarHandler::checkLowerThan(sideBars[i][i-1], currentLed, sideBars[i][i])) {
							leds[currentLed] = CRGB::DarkOrange;
						}
					} else {
						if(!VarHandler::checkHigherThan(sideBars[1][0], currentLed - starterLed, sideBars[1][1])) {
							leds[currentLed] = CRGB::DarkOrange;
						}
					}
				}
			}

			//Calculate runner indicator
			indRunLight++;
			if(((timer100b8 / indicatorPeriod) % 2 == 1) == indSwitch) {
				// Serial.println("switching");
				indRunLight = 0;
				indSwitch = !indSwitch;
			}
		}
	}

	void left(uint8_t c, uint8_t s)
	{
		if(s == 0) {
			if(c == 1) {
				indToggle[0] = !indToggle[0];
			}
		} else {

		}
	}

	void right(uint8_t c, uint8_t s)
	{
		if(s == 0) {
			if(c == 1) {
				indToggle[1] = !indToggle[1];
			}
		} else {

		}
	}

	void start()
	{
		indToggle[0] = false;
		indToggle[1] = false;
		indActivated = false;
		indRunLight = false;
	}

	//ability to change the speed it switches between colors
};