#include <FastLED.h>
#include "varHandler.h"

// #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN						5
#define NUM_LEDS						120
static const int LEFT_BUTTON_PIN = 		2;
static const int  RIGHT_BUTTON_PIN =	4;
static const int  SETTING_BUTTON_PIN =	3;
CRGB leds[NUM_LEDS];

#define FPS					120
uint8_t brightness = 		30;

uint8_t timer20b8 = 	random8();
uint8_t timer100b8 = 	random8();
uint8_t timer300b8 = 	random8();
uint8_t timer1000b8 = 	random8();

//Mode and Setting declarations
#include "mode.h"
#include "Bpm.h"
#include "Confetti.h"
#include "MainLights.h"
#include "RainbowWithGlitter.h"
#include "SolidRainbow.h"

int amtModes = 5;
Mode *modes[5] {
	new SolidRainbow(leds), 
	new RainbowWithGlitter(leds), 
	new Bpm(leds),
	new Confetti(leds),
	new MainLights(leds)
	};
uint8_t cMode = 4;

/*############################## Setting functions ##############################*/
void readButtons()
{
	uint8_t pins[3] = {LEFT_BUTTON_PIN, RIGHT_BUTTON_PIN, SETTING_BUTTON_PIN};
	static uint8_t buttonCounter[3] = {0, 0, 0};

	static uint8_t amtSettings = 4;
	static uint8_t cSetting = 0; // 0 = mode functions, 1 = mode settings, 2 = switch mode, 3 = brightness

	for (int i = 0; i < 3; ++i)
	{
		if (digitalRead(pins[i]) == HIGH) {
			buttonCounter[i]++;
			if (i == 2 && buttonCounter[i] == 1) {
				cSetting = (cSetting + 1) % amtSettings; // wrap around if exceeding amtSettings
				Serial.print("switching cSetting to ");
				Serial.print(cSetting);
				Serial.print("; buttonCounter: ");
				Serial.println(buttonCounter[i]);
			} else {
				switch (cSetting) {
					case 0:
					case 1: //activate left / right for the mode
						if (i == 0) modes[cMode]->left(buttonCounter[i], cSetting);
						else modes[cMode]->right(buttonCounter[i], cSetting);
						break;
					case 2: //switch mode
						if(buttonCounter[i] == 1) {
							//loop around after last mode
							cMode = (i == 0 ? (cMode - 1) : (cMode + 1)) % amtModes;
							modes[cMode]->start();
						}
						break;
					case 3: //adjust brightness
						brightness = VarHandler::capUint8(brightness, i == 0 ? -1 : 1);
						FastLED.setBrightness(brightness); //set brightness
						break;
					default:
						break;
				}
			}
		} else if (buttonCounter[i] > 0){
			buttonCounter[i] = 0;
		}
	}
}

/*############################## Arduino functions ##############################*/
void setup() 
{
	delay(1000); // some time for the led hardware to initialize
	Serial.begin(9600);

	//intitialize buttonpins
	pinMode(LEFT_BUTTON_PIN, INPUT);
	pinMode(RIGHT_BUTTON_PIN, INPUT);
	pinMode(SETTING_BUTTON_PIN, INPUT);

	//initialize fastled
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

	FastLED.setBrightness(brightness);
}

void loop() 
{
	modes[cMode]->render();

	FastLED.show();
	FastLED.delay(1000/FPS);

	//repeating vars
	EVERY_N_MILLISECONDS(20) { timer20b8++; }
	EVERY_N_MILLISECONDS(100) { timer100b8++; }
	EVERY_N_MILLISECONDS(300) { timer300b8++; }
	EVERY_N_SECONDS(1) { timer1000b8++; }

	//repeating functions
	EVERY_N_MILLISECONDS(75) { readButtons(); }

}