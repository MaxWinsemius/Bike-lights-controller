class SolidRainbow: public Mode {
public:
	SolidRainbow (CRGB *l) : Mode(l) {}
	void render()
	{
		fill_solid(&(leds[0]), NUM_LEDS, CHSV(timer20b8, 255, 255));
	}
};