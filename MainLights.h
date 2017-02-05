class MainLights: public Mode {
private:
	const uint8_t indicatorPeriod = 10;
	bool indToggle[2] = {false, false}; //indicator toggle; left, right
	bool indFinished[4] = {true};
	uint8_t indRunLight = 0; //indicator running light index
	bool indSwitch = false; //Timer to see when to restart

	int indSideParse[4] = {0, 1, 1, 0};

public:
	MainLights (CRGB *l) : Mode(l) {}

	void render()
	{
		//Main front light
		uint8_t h = calcSine(timer100b8, 149, 74, 0), 
				s = calcSine(timer1000b8, 200, 0, 0), 
				v = calcSine(timer100b8, 255, 198, 0);
		CHSV lightColor[2] = {CHSV(h, s, v), CHSV(0, 255, 217)};

		//fill front and back light
		for (int i = 0; i < 4; i += 2)
		{
			uint8_t startLed = !indToggle[0] && !indToggle[1] ? sideBars[i][0] : sideBars[i][1];
			uint8_t endLed = (!indToggle[0] && !indToggle[1] ? sideBars[i+1][1] : sideBars[i+1][0]) - startLed;
			fill_solid(&leds[startLed], endLed, lightColor[i/2]);
		}

		//indicators
		if(indToggle[0] || indToggle[1]) {
			//clear prev led val in blinker areas
			for (int i = 0; i < 4; ++i)
			{
			}

			//roll lights
			for (int i = 0; i < 4; ++i)
			{
				fadeLeds(sideBars[i][0], sideBars[i][1], 240);
				if(indToggle[indSideParse[i]]) {
				Serial.println(i);
					if(!indFinished[i]) {
						uint8_t starterLed = sideBars[i][1-indSideParse[i]];
						uint8_t currentLed = indSideParse[i] == 0 ? VarHandler::capUint8(starterLed, - (int16_t)indRunLight) : starterLed + indRunLight; // Calculate what led would turn on based on starter led

						bool rangeCheck = indSideParse[i] == 0 ? 
							!VarHandler::checkLowerThan(sideBars[i][1], currentLed, sideBars[i][0]) :
							!VarHandler::checkHigherThan(sideBars[i][0], currentLed - starterLed, sideBars[i][1]);

						if(rangeCheck) {
							leds[currentLed] = CRGB::DarkOrange;

							//make sure the last led of series turns off
							if(currentLed == sideBars[i][indSideParse[i]]) {
								indFinished[i] = true;
							}
						}
					}
				}
			}

			// //left indicators //begin #############
			// if(indToggle[0]) {
			// 	//front indicator
			// 	if(!indFinished[0]) {
			// 		uint8_t starterLed = sideBars[0][1];
			// 		uint8_t currentLed = VarHandler::capUint8(starterLed, - (int16_t)indRunLight);

			// 		if(!VarHandler::checkLowerThan(sideBars[0][1], currentLed, sideBars[0][0]));
			// 		leds[currentLed] = CRGB::DarkOrange;

			// 		//make sure the last led of series won't stay on
			// 		if(currentLed == sideBars[0][0]) {
			// 			indFinished[0] = true;
			// 		}
			// 	}

			// 	//back indicator
			// 	if(!indFinished[3]) {
			// 		uint8_t starterLed = sideBars[3][1];
			// 		uint8_t currentLed = VarHandler::capUint8(starterLed, - (int16_t)indRunLight);

			// 		if(!VarHandler::checkLowerThan(sideBars[3][1], currentLed, sideBars[3][0]));
			// 		leds[currentLed] = CRGB::DarkOrange;

			// 		//make sure the last led of series won't stay on
			// 		if(currentLed == sideBars[3][0]) {
			// 			indFinished[3] = true;
			// 		}
			// 	}
			// }

			// //right indicators
			// if(indToggle[1]) {
			// 	if(!indFinished[1]) {
			// 		uint8_t starterLed = sideBars[1][0]; // get first led of bar
			// 		uint8_t currentLed = starterLed + indRunLight; //Calculate what led would turn on based on starter led

			// 		if(!VarHandler::checkHigherThan(sideBars[1][0], currentLed - starterLed, sideBars[1][1])) {
			// 			leds[currentLed] = CRGB::DarkOrange;

			// 			//make sure the last led of series won't stay on
			// 			if(currentLed == sideBars[1][1] - 1) {
			// 				indFinished[1] = true;
			// 			}
			// 		}
			// 	}

			// 	if(!indFinished[2]) {
			// 		uint8_t starterLed = sideBars[2][0]; // get first led of bar
			// 		uint8_t currentLed = starterLed + indRunLight; //Calculate what led would turn on based on starter led

			// 		if(!VarHandler::checkHigherThan(sideBars[2][0], currentLed - starterLed, sideBars[2][1])) {
			// 			leds[currentLed] = CRGB::DarkOrange;

			// 			//make sure the last led of series won't stay on
			// 			if(currentLed == sideBars[2][1] - 1) {
			// 				indFinished[2] = true;
			// 			}
			// 		}
			// 	}
			// } //end #################

			//Calculate runner indicator
			indRunLight++;
			if(((timer100b8 / indicatorPeriod) % 2 == 1) == indSwitch) {
				indRunLight = 0;
				indSwitch = !indSwitch;
				indFinished[0] = indFinished[1] = indFinished[2] = indFinished[3] = false;
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