/*
 * funktioner.h
 *
 * Created: 09.01.2019 11:40:43
 *  Author: Kostya
 */ 


#ifndef FUNKTIONER_H_
#define FUNKTIONER_H_


extern byte gtemp[5];
extern byte k;

extern void init_io(void);
extern unsigned char SPI_Read(unsigned char reg);
extern void se8r01_powerup();
extern void se8r01_powerdown();
extern void se8r01_calibration();
extern void se8r01_setup();
extern void radio_settings(byte chanel);
extern unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value);
extern unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);
void rf_switch_bank(unsigned char bankindex);
void se8r01_send_pload(unsigned char *pBuf, unsigned char bytes);

#endif /* FUNKTIONER_H_ */