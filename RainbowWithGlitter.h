class RainbowWithGlitter: public Mode {
public:
	RainbowWithGlitter (CRGB *l) : Mode(l) {}

	void render()
	{
		int chanceOfGlitter = 80;

		// built-in FastLED rainbow, plus some random sparkly glitter
		fill_rainbow( leds, NUM_LEDS, timer20b8, 7);

		if(random8() < chanceOfGlitter) {
			leds[random16(NUM_LEDS)] += CRGB::White;
		}
	}
};