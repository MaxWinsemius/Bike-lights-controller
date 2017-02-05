class MainLights: public Mode {
private:
	const uint8_t indicatorPeriod = 10;
	bool indToggle[2] = {false, false}; //indicator toggle; left, right
	bool indFinished[4] = {false};
	uint8_t indRunLight = 0; //indicator running light index
	bool indSwitch = false; //Timer to see when to restart

public:
	MainLights (CRGB *l) : Mode(l) {}

	void render()
	{
		//Main front light
		uint8_t h = calcSine(timer100b8, 149, 74, 0), 
				s = calcSine(timer1000b8, 200, 0, 0), 
				v = calcSine(timer100b8, 255, 198, 0);
		CHSV clr = CHSV(h, s, v);

		//Fill front light part
		uint8_t startLed = !indToggle[0]&&!indToggle[1] ? sideBars[0][0] : sideBars[0][1];
		uint8_t endLed = (!indToggle[0]&&!indToggle[1] ? sideBars[1][1] : sideBars[1][0]) - startLed;
		fill_solid(&leds[startLed], endLed, clr);

		//indicators
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

					if(i == 0 && !indFinished[0]) {
						if(!VarHandler::checkLowerThan(sideBars[i][i-1], currentLed, sideBars[i][i])) {
							leds[currentLed] = CRGB::DarkOrange;

							//make sure the last led of series won't stay on
							if(currentLed == 0) {
								indFinished[0] = true;
							}
						}
					} else if (!indFinished[1]){
						if(!VarHandler::checkHigherThan(sideBars[1][0], currentLed - starterLed, sideBars[1][1])) {
							leds[currentLed] = CRGB::DarkOrange;

							//make sure the last led of series won't stay on
							if(currentLed == sideBars[1][1] - 1) {
								indFinished[1] = true;
							}
						}
					}
				}
			}

			//Calculate runner indicator
			indRunLight++;
			if(((timer100b8 / indicatorPeriod) % 2 == 1) == indSwitch) {
				indRunLight = 0;
				indSwitch = !indSwitch;
				indFinished[0] = indFinished[1] = false;
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
		indRunLight = false;
	}

	//ability to change the speed it switches between colors
};