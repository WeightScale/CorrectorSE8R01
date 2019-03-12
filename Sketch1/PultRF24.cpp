//this is a copy and paste job made by F2k
#include "Arduino.h"
#include "se8r01.h"
#include "funktioner.h"
#include "API.h"
#include "avr/wdt.h"
#include "avr/interrupt.h"

//#define DHTPIN 4 
//#define DHTTYPE DHT22   

//DHT dht(DHTPIN, DHTTYPE);


//***************************************************
#define TX_ADR_WIDTH    4   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  6  // 32 unsigned chars TX payload

unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = { 0x34,0x43,0x10,0x10 }; // Define a static TX address

void buttonfetch();
void wdt_in_enable();

byte massiv[TX_PLOAD_WIDTH];
bool isPressed;
byte buttons;
//long temp_time,temp_time1;
//byte mode ='t';      //r=rx, t=tx

unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};
//***************************************************
void setup(){
	DDRD = 0xFF & ~(1<<BUTTON0 | 1<<BUTTON1 | 1<<BUTTON2 | 1<<BUTTON3 | 1<<BUTTON4);
	PORTD = 0b00011111; //Pulling up a pin that is grounded will cause 90uA current leak
	ACSR = (1<<ACD); //Turn off Analog Comparator - this removes about 1uA
	ADCSRA = 0x00;//Turn off ADC
	PRR = 0xCF;		//Закоментировать перед использованием debugWire
	//PRR = 0xCB;	//Запрограммировать перед использованием debugWire
	//wdt_enable(WDTO_8S);
	cli();
	wdt_reset();
	//MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
	WDTCSR = (1<<WDE) | (1<<WDIE)|(1<<WDP3)|(1<<WDP0);	
	sei();
	
	pinMode(CEq,  OUTPUT);
	pinMode(SCKq, OUTPUT);
	pinMode(CSNq, OUTPUT);
	pinMode(MOSIq,  OUTPUT);
	pinMode(MISOq, INPUT);
	pinMode(IRQq, INPUT);
  
	init_io();                        // Initialize IO port
	unsigned char status=SPI_Read(STATUS); 

	digitalWrite(CEq, 0);
	delay(1);
	se8r01_powerup();
	se8r01_calibration();
	se8r01_setup();
	radio_settings(40);	//канал 40
	//SPI_RW_Reg(WRITE_REG|iRF_BANK0_CONFIG, 0x3E);
	digitalWrite(CEq, 1);
	se8r01_powerdown(); //Power down RF	  
	
	PCICR |=(1<<PCIE2);
	PCMSK2 = (1<<PCINT16)|(1<<PCINT17)|(1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	massiv[0] = '#';
	massiv[3] = '*';
}



void loop(){	
	buttonfetch();	
}

ISR(PCINT2_vect){		
}

ISR(WDT_vect){
	//wdt_reset();	
}

void buttonfetch(){
	if( (PIND & 0x1F) != 0x1F ){
		
		buttons = PIND & 0x1F;		
		digitalWrite(CEq, 0);	
		delay(1);
		//SPI_RW_Reg(WRITE_REG|iRF_BANK0_CONFIG, 0x3E);
		se8r01_powerup();
		delayMicroseconds(300);
		digitalWrite(CEq, 1);		
		uint16_t start_pressed = millis();
		uint16_t time_pressed = 0;		
		massiv[2] = false;
		
		while((PIND & 0x1F) != 0x1F){
			massiv[1] = PIND & 0x1F;
			time_pressed = millis()-start_pressed;
			massiv[4] = time_pressed >> 8;
			massiv[5] = time_pressed & 0xFF;
			if (time_pressed > 250){
				massiv[2] = true;
				se8r01_send_pload(massiv,TX_PLOAD_WIDTH);
				delay(100);	
			}
			wdt_reset();			
		};		
		
		if (!massiv[2])	{
			massiv[4] = time_pressed >> 8;
			massiv[5] = time_pressed & 0xFF;
			se8r01_send_pload(massiv,TX_PLOAD_WIDTH);		
		}
		se8r01_powerdown(); //Power down RF	
	}	
	
	//digitalWrite(CEq,LOW);
	//digitalWrite(CSNq,HIGH);
	
	wdt_in_enable();
	sleep_mode();	
	
}

void wdt_in_enable(){
	cli();
	wdt_reset();
	//MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);	
	WDTCSR |= (1<<WDE) | (1<<WDIE);
	//wdt_enable(WDTO_4S);
	sei();	
}

void radio_settings(byte chanel){
        
	SPI_RW_Reg(WRITE_REG|iRF_BANK0_EN_AA, 0x01);          //enable auto acc on pip 1
    SPI_RW_Reg(WRITE_REG|iRF_BANK0_EN_RXADDR, 0x01);      //enable pip 1
    SPI_RW_Reg(WRITE_REG|iRF_BANK0_SETUP_AW, 0x02);        //4 byte adress
        
    SPI_RW_Reg(WRITE_REG|iRF_BANK0_SETUP_RETR, B00001010);        //lowest 4 bits 0-15 rt transmisston higest 4 bits 256-4096us Auto Retransmit Delay
    SPI_RW_Reg(WRITE_REG|iRF_BANK0_RF_CH, chanel);
    SPI_RW_Reg(WRITE_REG|iRF_BANK0_RF_SETUP, 0x4f);        //2mps 0x4f
    //SPI_RW_Reg(WRITE_REG|iRF_BANK0_DYNPD, 0x01);          //pipe0 pipe1 enable dynamic payload length data
    //SPI_RW_Reg(WRITE_REG|iRF_BANK0_FEATURE, 0x07);        // enable dynamic paload lenght; enbale payload with ack enable w_tx_payload_noack
        
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);  //from tx
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
        
}
        
    
