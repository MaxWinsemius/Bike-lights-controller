class VarHandler {
private:
	VarHandler() {}
public:
	//cap the input + add between 0 and 255
	static uint8_t capUint8(uint8_t i, int16_t a)
	{
		int16_t aa = abs(a);
		uint8_t diffBot = i;
		uint8_t diffTop = 255 - i;
		if(a >= 0) {
			if(aa > diffTop) return 255; //check if i+a > 255
		} else {
			if(aa > diffBot) return 0; //check if i-a < 0
		}
		return i + a; //return sum
	}

	//returns true if index + add is higher than m
	static bool checkHigherThan(uint8_t i, uint8_t a, uint8_t m)
	{
		uint16_t index = (uint16_t)i;
		uint16_t add = (uint16_t)a;
		uint16_t max = (uint16_t)m;

		return (index + a) >= m;
	}

	//returns true if index - add is lower than m
	static bool checkLowerThan(uint8_t i, uint8_t a, uint8_t m)
	{
		uint16_t index = (uint16_t)i;
		uint16_t add = (uint16_t)a;
		uint16_t max = (uint16_t)m;

		return (index - a) < m;
	}
};