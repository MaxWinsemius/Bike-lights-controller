class Confetti: public Mode {
public:
	Confetti (CRGB *l) : Mode(l) {}

	void render()
	{
		// random colored speckles that blink in and fade smoothly
	  	fadeLeds( 0, NUM_LEDS, 10);
	  	int pos = random16(NUM_LEDS);
	  	leds[pos] += CHSV( timer20b8 + random8(64), 200, 255);
	}
};