// CAT5171.h

#ifndef _CAT5171_h
#define _CAT5171_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Wire.h>

class CAT5171{
	private:
		int _addr;
	
	public:
		/*! Класс цифрового потенциометра */
		CAT5171();
		/*! Класс цифрового потенциометра */
		CAT5171(int addr);
		void setResistance(uint8_t);
		uint8_t getResistance();
		void shutdown();
		void reset();
		void twitch(uint8_t, long );		
	
};


#define CAT5171_AD0 0x2C /*! Адрес AD0 семейства CAT5171  */
#define CAT5171_AD1 0x2D /*! Адрес AD1 семейства CAT5171  */

#define CAT5171_SHUTDOWN	0x20
#define CAT5171_RESET		0x40

#endif

