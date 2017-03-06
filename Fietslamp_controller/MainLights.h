class MainLights: public Mode {
private:

	//setting vars
	uint8_t extensiveness = 0; //Simple, extensive, knight rider
	uint8_t amntExtensiveness = 3;
	uint8_t knightRiderSpeed = 1;

	//knight rider vars
	uint8_t lastLed[2] = {sideBars[0][0], sideBars[2][0]};
	uint8_t counter = 0;

	//indicator vars
	const uint8_t indicatorPeriod = 8; //Periods n^2 work best (they don't skip beats).
	const uint8_t indicatorSimpleWidth = 8;
	bool indToggle[2] = {false, false}; //indicator toggle; left, right
	bool indOn = false;
	bool indLastState = indOn;
	bool indFinished[4] = {true};
	uint8_t indRunLight = 0; //indicator running light index
	bool indSwitch = false; //Timer to see when to restart

	int indSideParse[4] = {0, 1, 1, 0};

	uint8_t calcPartitionalPoint(uint8_t t1, uint8_t t2, uint8_t p) {
		uint16_t total1 = (uint16_t)t1;
		uint16_t total2 = (uint16_t)t2;
		uint16_t point = (uint16_t)p;

		return (uint8_t)(t1 * p / t2);
	}

	void switchIndicators()
	{
		indOn = indToggle[0] || indToggle[1];

		if (indOn != indLastState) { //only run if switched
			if(indOn) {
				knightRiderSpeed = knightRiderSpeed * 2;
			} else {
				knightRiderSpeed = knightRiderSpeed / 2;
			}
		}
		indLastState = indOn;
	}

public:
	//construct
	MainLights (CRGB *l) : Mode(l) {
		amntSettings = 1;
	}

	void render()
	{
		static const uint8_t defaultFade = 240, knightFade = 250;
		if(extensiveness != 2) { 
			fadeLeds(0, NUM_LEDS, defaultFade);
		} else if(!indOn) {
			fadeLeds(0, NUM_LEDS, knightFade);
		} else {
			fadeLeds(0, sideBars[0][1], defaultFade);
			fadeLeds(sideBars[0][1], sideBars[1][0], knightFade);
			fadeLeds(sideBars[1][0], sideBars[2][1], defaultFade);
			fadeLeds(sideBars[2][1], sideBars[3][0], knightFade);
			fadeLeds(sideBars[3][0], sideBars[3][1], defaultFade);
		}

		if(extensiveness == 1 || extensiveness == 2) {
			//Main front light
			uint8_t h = calcSine(timer100b8, 149, 74, 0), 
					s = calcSine(timer1000b8, 200, 0, 0), 
					v = calcSine(timer100b8, 255, 198, 0);
			CHSV lightColor[2] = {CHSV(h, s, v), CHSV(0, 255, 217)};

			//fill front and back light
			if(extensiveness == 1) {
				//extensive
				for (int i = 0; i < 4; i += 2)
				{
					uint8_t startLed = sideBars[i][indOn];
					uint8_t endLed = sideBars[i+1][!indOn]- startLed;
					fill_solid(&leds[startLed], endLed, lightColor[i/2]);
				}
			} else { //knight rider
				for(uint8_t i; i < 2; i++) {
					uint8_t runner = calcTri((uint16_t)counter, (uint16_t)(sideBars[i * 2 + 1][!indOn]), (uint16_t)sideBars[i * 2][indOn], 0);

					if(runner > lastLed[i]) {
						fill_solid(&leds[lastLed[i] + sideBars[i * 2][indOn]], runner - lastLed[i] + 1, lightColor[i]);
					} else {
						fill_solid(&leds[runner + sideBars[i * 2][indOn]], lastLed[i] - runner + 1, lightColor[i]);
					}

					lastLed[i] = runner;
				}

				counter += knightRiderSpeed;
			}
		} else { //Simple
			//fill front light
			fill_solid(&leds[30 - (indicatorSimpleWidth / 2)], indicatorSimpleWidth, CHSV(0, 0, 200));

			//fill back light
			fill_solid(&leds[(sideBars[3][1] - (sideBars[3][1] - 60) / 2) - (indicatorSimpleWidth / 2)], indicatorSimpleWidth, CHSV(0, 255, 217));
		}

		//indicators
		if(indOn) {
			//roll lights
			for (int i = 0; i < 4; ++i)
			{
				//check which side to show
				if(indToggle[indSideParse[i]]) {
					if(extensiveness == 1 || extensiveness == 2) {
						if(!indFinished[i]) { //check if the indicator has finished running
							indFinished[i] = blinkExtensive(sideBars[i], indRunLight, indSideParse[i], CRGB::DarkOrange); //blink led
						}
					} else if(indSwitch && extensiveness == 0) {
						//Simple
						uint8_t startLed = i%2 == 0 ? sideBars[i][1] - indicatorSimpleWidth : sideBars[i][0];
						fill_solid(&leds[startLed], indicatorSimpleWidth, CRGB::DarkOrange);
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
				switchIndicators();
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
				switchIndicators();
			} else {
				switch(cSetting) {
					case 0:
						extensiveness = (extensiveness + 1) % amntExtensiveness; //wrap around;

						if(extensiveness == 2) {
							counter = 0;
							for(uint8_t i = 0; i < 2; i++) {
								lastLed[i] = calcTri((uint16_t)counter, (uint16_t)(sideBars[i * 2 + 1][!indOn]), (uint16_t)sideBars[i * 2][indOn], 0); 
							}
						}
						break;
					case 1:
					
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