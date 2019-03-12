#include "RadioClass.h"

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10}; // Define a static TX address
//unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
//unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

void RadioClass::init(){

	pinMode(CEq,  OUTPUT);
	pinMode(SCKq, OUTPUT);
	pinMode(CSNq, OUTPUT);
	pinMode(MOSIq,  OUTPUT);
	pinMode(MISOq, INPUT);
	pinMode(IRQq, INPUT);
	
	digitalWrite(IRQq, 0);
	digitalWrite(CEq, 0);			// chip enable
	digitalWrite(CSNq, 1);                 // Spi disable

	unsigned char status=spi_read(STATUS); 
	
	digitalWrite(CEq, 0);
	delay(1);
	powerup();
	calibration();
	setup();
	settings(40);
	spi_rw_reg(WRITE_REG|iRF_BANK0_CONFIG, 0x3f);
	digitalWrite(CEq, 1);
	
	PCICR |=(1<<PCIE2);
	PCMSK2 |=(1<<PCINT23);
}

void RadioClass::powerup(){
	rf_switch_bank(iBANK0);
	spi_rw_reg(iRF_CMD_WRITE_REG|iRF_BANK0_CONFIG,0x03);
	spi_rw_reg(iRF_CMD_WRITE_REG|iRF_BANK0_RF_CH,0x32);
	spi_rw_reg(iRF_CMD_WRITE_REG|iRF_BANK0_RF_SETUP,0x48);
	spi_rw_reg(iRF_CMD_WRITE_REG|iRF_BANK0_PRE_GURD,0x77); //2450 calibration
}

void RadioClass::calibration(){
	rf_switch_bank(iBANK1);

	gtemp[0]=0x40;
	gtemp[1]=0x00;
	gtemp[2]=0x10;
	gtemp[3]=0xE6;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_PLL_CTL0, gtemp, 4);

	gtemp[0]=0x20;
	gtemp[1]=0x08;
	gtemp[2]=0x50;
	gtemp[3]=0x40;
	gtemp[4]=0x50;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_CAL_CTL, gtemp, 5);

	gtemp[0]=0x00;
	gtemp[1]=0x00;
	gtemp[2]=0x1E;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_IF_FREQ, gtemp, 3);

	gtemp[0]=0x29;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_FDEV, gtemp, 1);

	gtemp[0]=0x00;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_DAC_CAL_LOW, gtemp, 1);

	gtemp[0]=0x7F;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_DAC_CAL_HI, gtemp, 1);

	gtemp[0]=0x02;
	gtemp[1]=0xC1;
	gtemp[2]=0xEB;
	gtemp[3]=0x1C;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_AGC_GAIN, gtemp, 4);

	gtemp[0]=0x97;
	gtemp[1]=0x64;
	gtemp[2]=0x00;
	gtemp[3]=0x81;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_RF_IVGEN, gtemp, 4);

	rf_switch_bank(iBANK0);

	digitalWrite(CEq, 1);
	delayMicroseconds(30);
	digitalWrite(CEq, 0);

	delayMicroseconds(50);                            // delay 50ms waitting for calibaration.

	digitalWrite(CEq, 1);
	delayMicroseconds(30);
	digitalWrite(CEq, 0);

	delayMicroseconds(50);                            // delay 50ms waitting for calibaration.
	// calibration end
}

void RadioClass::setup(){
	
	gtemp[0]=0x28;
	gtemp[1]=0x32;
	gtemp[2]=0x80;
	gtemp[3]=0x90;
	gtemp[4]=0x00;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK0_SETUP_VALUE, gtemp, 5);

	delayMicroseconds(2);

	rf_switch_bank(iBANK1);

	gtemp[0]=0x40;
	gtemp[1]=0x01;
	gtemp[2]=0x30;
	gtemp[3]=0xE2;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_PLL_CTL0, gtemp, 4);

	gtemp[0]=0x29;
	gtemp[1]=0x89;
	gtemp[2]=0x55;
	gtemp[3]=0x40;
	gtemp[4]=0x50;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_CAL_CTL, gtemp, 5);

	gtemp[0]=0x29;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_FDEV, gtemp, 1);

	gtemp[0]=0x55;
	gtemp[1]=0xC2;
	gtemp[2]=0x09;
	gtemp[3]=0xAC;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_RX_CTRL, gtemp, 4);

	gtemp[0]=0x00;
	gtemp[1]=0x14;
	gtemp[2]=0x08;
	gtemp[3]=0x29;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_FAGC_CTRL_1, gtemp, 4);

	gtemp[0]=0x02;
	gtemp[1]=0xC1;
	gtemp[2]=0xCB;
	gtemp[3]=0x1C;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_AGC_GAIN, gtemp, 4);

	gtemp[0]=0x97;
	gtemp[1]=0x64;
	gtemp[2]=0x00;
	gtemp[3]=0x01;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_RF_IVGEN, gtemp, 4);

	gtemp[0]=0x2A;
	gtemp[1]=0x04;
	gtemp[2]=0x00;
	gtemp[3]=0x7D;
	spi_write_buf(iRF_CMD_WRITE_REG|iRF_BANK1_TEST_PKDET, gtemp, 4);

	rf_switch_bank(iBANK0);
}

void RadioClass::settings(byte chanel){
	
	spi_rw_reg(WRITE_REG|iRF_BANK0_EN_AA, 0x01);          //enable auto acc on pip 1
	spi_rw_reg(WRITE_REG|iRF_BANK0_EN_RXADDR, 0x01);      //enable pip 1
	spi_rw_reg(WRITE_REG|iRF_BANK0_SETUP_AW, 0x02);        //4 byte adress
	
	spi_rw_reg(WRITE_REG|iRF_BANK0_SETUP_RETR, B00001010);        //lowest 4 bits 0-15 rt transmisston higest 4 bits 256-4096us Auto Retransmit Delay
	spi_rw_reg(WRITE_REG|iRF_BANK0_RF_CH, chanel);
	spi_rw_reg(WRITE_REG|iRF_BANK0_RF_SETUP, 0x4f);        //2mps 0x4f
	//spi_rw_reg(WRITE_REG|iRF_BANK0_DYNPD, 0x01);          //pipe0 pipe1 enable dynamic payload length data
	//spi_rw_reg(WRITE_REG|iRF_BANK0_FEATURE, 0x07);        // enable dynamic paload lenght; enbale payload with ack enable w_tx_payload_noack
	
	spi_write_buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);  //from tx
	spi_write_buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device
	spi_rw_reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
	
}

/**************************************************
 * Function: TX_Mode();
 * 
 * Description:
 * This function initializes one nRF24L01 device to
 * TX mode, set TX address, set RX address for auto.ack,
 * fill TX payload, select RF channel, datarate & TX pwr.
 * PWR_UP is set, CRC(2 unsigned chars) is enabled, & PRIM:TX.
 * 
 * ToDo: One high pulse(>10us) on CE will now send this
 * packet and expext an acknowledgment from the RX device.
 **************************************************/
void RadioClass::rf_switch_bank(unsigned char bankindex){
    unsigned char temp0,temp1;
    temp1 = bankindex;

    temp0 = spi_rw(iRF_BANK0_STATUS);

    if((temp0&0x80)!=temp1)
    {
        spi_rw_reg(iRF_CMD_ACTIVATE,0x53);
    }
}

/**************************************************
 * Function: spi_rw();
 * 
 * Description:
 * Writes one unsigned char to nRF24L01, and return the unsigned char read
 * from nRF24L01 during write, according to SPI protocol
 **************************************************/
unsigned char RadioClass::spi_rw(unsigned char Byte){
	unsigned char i;
	for(i=0;i<8;i++){                      // output 8-bit  
		if(Byte&0x80) {
			digitalWrite(MOSIq, 1);
		} else {
			digitalWrite(MOSIq, 0);
		}
		digitalWrite(SCKq, 1);
		Byte <<= 1;                         // shift next bit into MSB..
		if(digitalRead(MISOq) == 1) {
			Byte |= 1;       	                // capture current MISO bit
		}
		digitalWrite(SCKq, 0);
	}
	return(Byte);           	        // return read unsigned char
}

/**************************************************
 * Function: spi_rw_reg();
 * 
 * Description:
 * Writes value 'value' to register 'reg'
/**************************************************/
unsigned char RadioClass::spi_rw_reg(unsigned char reg, unsigned char value){
  unsigned char status;

  digitalWrite(CSNq, 0);                   // CSN low, init SPI transaction
  status = spi_rw(reg);                   // select register
  spi_rw(value);                          // ..and write value to it..
  digitalWrite(CSNq, 1);                   // CSN high again

  return(status);                   // return nRF24L01 status unsigned char
}

/**************************************************
 * Function: spi_read();
 * 
 * Description:
 * Read one unsigned char from nRF24L01 register, 'reg'
/**************************************************/
unsigned char RadioClass::spi_read(unsigned char reg){
  unsigned char reg_val;

  digitalWrite(CSNq, 0);           // CSN low, initialize SPI communication...
  spi_rw(reg);                   // Select register to read from..
  reg_val = spi_rw(0);           // ..then read register value
  digitalWrite(CSNq, 1);          // CSN high, terminate SPI communication
  
  return(reg_val);               // return register value
}

/**************************************************
 * Function: spi_read_buf();
 * 
 * Description:
 * Reads 'unsigned chars' #of unsigned chars from register 'reg'
 * Typically used to read RX payload, Rx/Tx address
/**************************************************/
unsigned char RadioClass::spi_read_buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes){
  unsigned char status,i;

  digitalWrite(CSNq, 0);                  // Set CSN low, init SPI tranaction
  status = spi_rw(reg);       	    // Select register to write to and read status unsigned char

  for(i=0;i<bytes;i++){
    pBuf[i] = spi_rw(0);    // Perform SPI_RW to read unsigned char from nRF24L01
  }

  digitalWrite(CSNq, 1);                   // Set CSN high again

  return(status);                  // return nRF24L01 status unsigned char
}

/**************************************************
 * Function: spi_write_buf();
 * 
 * Description:
 * Writes contents of buffer '*pBuf' to nRF24L01
 * Typically used to write TX payload, Rx/Tx address
/**************************************************/
unsigned char RadioClass::spi_write_buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes){
  unsigned char status,i;

  digitalWrite(CSNq, 0);                  // Set CSN low, init SPI tranaction
  status = spi_rw(reg);             // Select register to write to and read status unsigned char
  for(i=0;i<bytes; i++){             // then write all unsigned char in buffer(*pBuf)  
    spi_rw(*pBuf++);
  }
  digitalWrite(CSNq, 1);                   // Set CSN high again
  return(status);                  // return nRF24L01 status unsigned char
}

void RadioClass::parceData(){
	unsigned char status = spi_read(STATUS);
	delay(1);//read reg too close after irq low not good
	if(status&STA_MARK_RX){                                                 // if receive data ready (TX_DS) interrupt
		spi_read_buf(RD_RX_PLOAD, rx_buf, TX_PLOAD_WIDTH);             // read playload to rx_buf
		spi_rw_reg(FLUSH_RX,0); // clear RX_FIFO
		if(rx_buf[0] == '#' && rx_buf[3]=='*'){
			spi_rw_reg(WRITE_REG+STATUS,0xff);
			_eventHandler(rx_buf,TX_PLOAD_WIDTH);
		}
	}else{
		spi_rw_reg(WRITE_REG+STATUS,0xff);
	}
	
}

ISR(PCINT2_vect){
	if(digitalRead(IRQq)==LOW){
		SE8R01.parceData();
	}
}

RadioClass SE8R01;

