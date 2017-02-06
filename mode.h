class Mode {
protected:
	CRGB *leds;
	uint8_t sideBars[4][2] = {{0, 19}, {40, 60}, {60, 77}, {95, 112}}; //front left, front right, back left, back right

	uint8_t calcSine(uint16_t counter, uint16_t top, uint16_t bottom, uint16_t sineOffset)
	{
		uint16_t map = top - bottom;
		return (uint8_t)(((uint16_t)(sin8(counter+sineOffset))*map)/255)+bottom;
	}

	void fadeLeds(uint8_t start, uint8_t end, uint8_t scale)
	{
		for (int i = start; i < end; ++i)
		{
			leds[i].nscale8(scale);
		}
	}
	
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

public:
	Mode(CRGB *l) { leds = l; }

	virtual void render() {};
	virtual void start() {}; //run stuff when this mode is switched to
	virtual void left(uint8_t c, uint8_t s) {}; //run this when the left/right button is being pushed
	virtual void right(uint8_t c, uint8_t s) {};


};