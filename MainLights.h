class MainLights: public Mode {
private:
	//setting vars
	uint8_t cSetting = 0; //Switch extensiveness, [more options here]
	const uint8_t amntSettings = 1;
	uint8_t extensiveness = 0; //Simple, extensive, knight rider
	uint8_t amntExtensiveness = 3;

	//indicator vars
	const uint8_t indicatorPeriod = 8; //Periods n^2 work best (don't skip beats).
	const uint8_t indicatorSimpleWidth = 8;
	bool indToggle[2] = {false, false}; //indicator toggle; left, right
	bool indFinished[4] = {true};
	uint8_t indRunLight = 0; //indicator running light index
	bool indSwitch = false; //Timer to see when to restart

	int indSideParse[4] = {0, 1, 1, 0};
	bool indSideParseB[4] = {false, true, true, false};


	//indicator methods
	bool blinkExtensive(uint8_t bar[2], uint8_t runner, bool direction)
	{
		//calculate current led position based on direction
		uint8_t currentLed = direction ? bar[0] + runner : bar[1] - 1 - runner;

		//set led color
		leds[currentLed] = CRGB::DarkOrange;

		//check if the led has exceeded the last led to blink (to prevent it trying to color too many leds)
		return direction ? 
			VarHandler::checkHigherThan(bar[0], runner, bar[1] - 1) : 
			VarHandler::checkLowerThan(bar[1], runner, bar[0] + 2);
	}

	void blinkSimple(uint8_t startLed, uint8_t width) {
		for (uint8_t i = startLed; i < startLed+width; ++i) leds[i] = CRGB::DarkOrange;
	}

public:
	//construct
	MainLights (CRGB *l) : Mode(l) {}

	void render()
	{
		//fade from last round
		for (int i = 0; i < 4; ++i)
		{
			fadeLeds(sideBars[i][0], sideBars[i][1], 240);
		}

		if(extensive) {
			//Main front light
			uint8_t h = calcSine(timer100b8, 149, 74, 0), 
					s = calcSine(timer1000b8, 200, 0, 0), 
					v = calcSine(timer100b8, 255, 198, 0);
			CHSV lightColor[2] = {CHSV(h, s, v), CHSV(0, 255, 217)};

			//fill front and back light
			for (int i = 0; i < 4; i += 2)
			{
				uint8_t startLed = (!indToggle[0] && !indToggle[1] ? sideBars[i][0] : sideBars[i][1]);
				uint8_t endLed = (!indToggle[0] && !indToggle[1] ? sideBars[i+1][1] : sideBars[i+1][0]) - startLed;
				fill_solid(&leds[startLed], endLed, lightColor[i/2]);
			}
		} else { //Simple
			//fill front light
			fill_solid(&leds[30 - (indicatorSimpleWidth / 2)], indicatorSimpleWidth, CHSV(0, 0, 200));

			//fill back light
			fill_solid(&leds[(sideBars[3][1] - (sideBars[3][1] - 60) / 2) - (indicatorSimpleWidth / 2)], indicatorSimpleWidth, CHSV(0, 255, 217));
		}

		//indicators
		if(indToggle[0] || indToggle[1]) {

			//roll lights
			for (int i = 0; i < 4; ++i)
			{

				//check which side to show
				if(indToggle[indSideParse[i]]) {
					if(extensiveness == 1 || extensiveness == 2) {
						if(!indFinished[i]) { //check if the indicator has finished running
							indFinished[i] = blinkExtensive(sideBars[i], indRunLight, indSideParseB[i]); //blink led
						}
					} else if(indSwitch && extensiveness == 0) {
						//Simple
						uint8_t startLed = i%2 == 0 ? sideBars[i][1] - indicatorSimpleWidth : sideBars[i][0];
						blinkSimple(startLed, indicatorSimpleWidth);
					}
				}
			}

			//Calculate runner indicator
			indRunLight++;

			//check if next round for indicator blink should be run, and if so reset loads of vars.
			if(((timer100b8 / indicatorPeriod) % 2 == 1) == indSwitch) {
				indRunLight = 0;
				indSwitch = !indSwitch;
				if(extensiveness == 1 || extensiveness == 2) {
					indFinished[0] = indFinished[1] = indFinished[2] = indFinished[3] = false;
				}
			}
		}
	}

	void left(uint8_t c, uint8_t s)
	{
		if(c == 1) {
			if(s == 0) {
				indToggle[0] = !indToggle[0]; //Turn on toggle for left side.
			} else {
				//Switch cSetting
				cSetting = (cSetting + 1) % amntSettings; //warp around if exceeding amntSettings
			}
		}
	}

	void right(uint8_t c, uint8_t s)
	{
		if(c == 1) {
			if(s == 0) {
				indToggle[1] = !indToggle[1]; //Turn on toggle for right side.
			} else {
				switch(cSetting) {
					case 0:
						extensiveness = (extensiveness + 1) % amntExtensiveness; //wrap around;
						break;
					default:
						break;
				}
			}
		}
	}

	void start()
	{
		indToggle[0] = false;
		indToggle[1] = false;
		indRunLight = false;
	}

	//ability to change the speed it switches between colors
};