class MainLights: public Mode {
private:
	//setting vars
	uint8_t cSetting = 0; //Switch extensiveness, [more options here]
	const uint8_t amntSettings = 1;
	uint8_t extensiveness = 2; //Simple, extensive, knight rider
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

	uint8_t calcPartitionalPoint(uint8_t t1, uint8_t t2, uint8_t p) {
		uint16_t total1 = (uint16_t)t1;
		uint16_t total2 = (uint16_t)t2;
		uint16_t point = (uint16_t)p;

		return (uint8_t)(t1 * p / t2);
	}

public:
	//construct
	MainLights (CRGB *l) : Mode(l) {}

	void render()
	{
		static const uint8_t quickFade = 240, slowFade = 250;
		if(extensiveness != 2) { 
			fadeLeds(0, NUM_LEDS, quickFade);
		} else if(!indToggle[0] && !indToggle[1]) {
			fadeLeds(0, NUM_LEDS, slowFade);
		} else {
			fadeLeds(0, sideBars[0][1], quickFade);
			fadeLeds(sideBars[0][1], sideBars[1][0], slowFade);
			fadeLeds(sideBars[1][0], sideBars[2][1], quickFade);
			fadeLeds(sideBars[2][1], sideBars[3][0], slowFade);
			fadeLeds(sideBars[3][0], sideBars[3][1], quickFade);
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
					uint8_t startLed = (!indToggle[0] && !indToggle[1] ? sideBars[i][0] : sideBars[i][1]);
					uint8_t endLed = (!indToggle[0] && !indToggle[1] ? sideBars[i+1][1] : sideBars[i+1][0]) - startLed;
					fill_solid(&leds[startLed], endLed, lightColor[i/2]);
				}
			} else {
				//knight rider

				// uint8_t runner = (uint8_t)(((int16_t)triwave8(timer20b8) * (int16_t)NUM_LEDS) / 255);
				// leds[runner] = lightColor[0];
				static uint8_t counter = 0;

				uint8_t index = (indToggle[0] || indToggle[1]) ? 1 : 0;

				int16_t frontWidth = (int16_t)(sideBars[1][1 - index] - sideBars[0][index]);
				uint8_t frontRunner = sideBars[0][index] + (uint8_t)(((int16_t)triwave8(counter) * frontWidth) / 255);
				int16_t backWidth = (int16_t)(sideBars[3][1 - index] - sideBars[2][index]);
				uint8_t backRunner = sideBars[2][index] + (uint8_t)(((int16_t)triwave8(counter) * backWidth) / 255);

				leds[frontRunner] = lightColor[0];
				leds[backRunner] = lightColor[1];

				counter++;
				// uint8_t width[2];
				// if(indToggle[0] || indToggle[1]) {
				// 	width[0] = sideBars[1][0] - sideBars[0][1];
				// 	width[1] = sideBars[3][0] - sideBars[2][1];
				// } else {
				// 	width[0] = sideBars[1][1] - sideBars[0][0];
				// 	width[1] = sideBars[3][1] - sideBars[2][0];
				// }
				// uint8_t frontRunner = (uint8_t)((int16_t)triwave8(timer20b8) * (int16_t)width[0] / 255);
				// uint8_t backRunner = (uint8_t)((int16_t)triwave8(timer20b8) * (int16_t)width[1] / 255);
				// blinkExtensive()
				// uint8_t frontRunner = sideBars[0][1] + 1 + (calcPartitionalPoint(width[0], triwave8(timer20b8), 255));
				// uint8_t backRunner = sideBars[2][1] + 1 + (calcPartitionalPoint(width[1], triwave8(timer20b8), 255));
				// leds[frontRunner] = lightColor[0];
				// leds[backRunner] = lightColor[1];

				// for(uint8_t i = 0; i < 2; i++) {
				// 	uint8_t runner = sideBars[i*2][indToggle[0] || indToggle[1] ? 1 : 0] + 1 + (calcPartitionalPoint(width[i], triwave8(timer20b8), 255));
				// 	leds[runner] = lightColor[i];
				// }
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
							indFinished[i] = blinkExtensive(sideBars[i], indRunLight, indSideParseB[i], CRGB::DarkOrange); //blink led
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