#include <SPI.h>
#include <NRFLite.h>
#include <avr/interrupt.h>

NRFLite _radio;
uint8_t _data;
void radioInterrupt();

void setup(){
	pinMode(PD7,INPUT_PULLUP);
	PCICR |=(1<<PCIE2);
	PCMSK2 |=(1<<PCINT23);
	_radio.init(0, PD5, SS); // Set transmitter radio to Id = 0, along with the CE and CSN pins
	//attachInterrupt(digitalPinToInterrupt(PD7), radioInterrupt, FALLING);
}

void loop()
{
	_data++; // Change some data.
	_radio.startSend(1, &_data, sizeof(_data)); // Send to the radio with Id = 1
	delay(1000);
}

ISR(PCINT2_vect){
	if(digitalRead(PD7) == LOW){
		radioInterrupt();
	}
}

void radioInterrupt()
{
	// Ask the radio what caused the interrupt.
	// txOk = the radio successfully transmitted data.
	// txFail = the radio failed to transmit data.
	// rxReady = the radio has received data.
	uint8_t txOk, txFail, rxReady;
	_radio.whatHappened(txOk, txFail, rxReady);

	if (txOk)
	{
		_data = txOk;
	}

	if (txFail)
	{
		_data = txFail;
	}
}