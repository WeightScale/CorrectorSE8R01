// HX711.h

#ifndef _HX711_h
#define _HX711_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define PIN_DATA 8
#define PIN_CLOCK 9

class Q2HX711{
	
	private:
		byte CLOCK_PIN;
		byte OUT_PIN;
		byte GAIN;
		bool pinsConfigured;

	public:
		Q2HX711(byte output_pin, byte clock_pin);
		virtual ~Q2HX711();
		bool readyToSend();
		void setGain(byte gain = 128);
		long read();
		void powerDown();
		void powerUp();
};

extern Q2HX711 hx711;

#endif

