class Bpm: public Mode {
public:
	Bpm (CRGB *l) : Mode(l) {}

	void render()
	{
		// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
		uint8_t BeatsPerMinute = 62;
		CRGBPalette16 palette = PartyColors_p;
		uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
		for( int i = 0; i < NUM_LEDS; i++) { //9948
			leds[i] = ColorFromPalette(palette, timer20b8+(i*2), beat-timer20b8+(i*10));
		}
	}
};