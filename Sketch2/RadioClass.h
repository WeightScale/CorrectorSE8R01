#ifndef _RadioClass_h_
#define _RadioClass_h_

#include <Arduino.h>
//#include <functional>
#include "API.h"

//---------------------------------------------
#define CEq       5				//PD5
// CE_BIT:   Digital Input     Chip Enable Activates RX or TX mode
#define CSNq      10			//PB2
// CSN BIT:  Digital Input     SPI Chip Select
#define SCKq      13
// SCK BIT:  Digital Input     SPI Clock
#define MOSIq     11
// MOSI BIT: Digital Input     SPI Slave Data Input
#define MISOq     12
// MISO BIT: Digital Output    SPI Slave Data Output, with tri-state option
#define IRQq      7
// IRQ BIT:  Digital Output    Maskable interrupt pin
//*********************************************
#define TX_PLOAD_WIDTH  6  // 32 unsigned chars TX payload6
#define TX_ADR_WIDTH    4   // 5 unsigned chars TX(RX) address width4

	
//typedef std::function<void(uint8_t *data, size_t len)> EventHandler;
typedef void (*EventHandler)(uint8_t *data, size_t len);	

class RadioClass{
private:
	EventHandler _eventHandler;
	
protected:
	byte gtemp[5];
	//byte k=0;
	unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
	unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};
	unsigned char spi_rw(unsigned char Byte);
	unsigned char spi_rw_reg(unsigned char reg, unsigned char value);
	unsigned char spi_read(unsigned char reg);
	unsigned char spi_read_buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);
	unsigned char spi_write_buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);
	
public:	
	void init();
	void powerup();
	void calibration();
	void setup();
	void settings(byte chanel);
	void rf_switch_bank(unsigned char bankindex);
	void parceData();
	void onCallBack(EventHandler event){_eventHandler = event;};		
};

extern unsigned char TX_ADDRESS[TX_ADR_WIDTH];
//extern unsigned char rx_buf[TX_PLOAD_WIDTH]; // initialize value
//extern unsigned char tx_buf[TX_PLOAD_WIDTH];
		
extern RadioClass SE8R01;

#endif
