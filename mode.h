class Mode {
protected:
	CRGB *leds;
	uint8_t sideBars[4][2] = {{0, 19}, {40, 59}, {60, 77}, {95, 112}}; //front left, front right, back left, back right

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

public:
	Mode(CRGB *l) { leds = l; }

	virtual void render() {};
	virtual void start() {};
	virtual void left(uint8_t c, uint8_t s) {};
	virtual void right(uint8_t c, uint8_t s) {};


};