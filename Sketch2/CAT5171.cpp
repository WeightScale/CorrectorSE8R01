#include "CAT5171.h"

/*! Класс цифрового потенциометра */
CAT5171::CAT5171(){
	_addr = CAT5171_AD0;	
};
	
CAT5171::CAT5171(int addr){
	_addr = addr;	
};

void CAT5171::setResistance(uint8_t date){
	Wire.beginTransmission(_addr); 	
	Wire.write(0);             
	Wire.write(date);
	Wire.endTransmission();     
};

void CAT5171::shutdown(){
	Wire.beginTransmission(_addr);
	Wire.write(CAT5171_SHUTDOWN); 
	Wire.write(0);
	Wire.endTransmission();
};

void CAT5171::reset(){
	Wire.beginTransmission(_addr);
	Wire.write(CAT5171_RESET);
	Wire.write(0);
	Wire.endTransmission();
};

uint8_t CAT5171::getResistance(){
	uint8_t c;
	Wire.requestFrom(_addr, 8);		///< запрашиваем у ведомого устройства #2 8 байт 
	while(Wire.available()){		///< ведомое устройство может отправить не все запрашиваемые байты  
		c = Wire.read();			///< считываем байт данных в виде символа
	}
	return c;
}

void CAT5171::twitch(uint8_t c, long t){
	setResistance(getResistance() + c);								///< Для визуального определения что чтото изменилось.
	delay(t);
	setResistance(getResistance() - c);	
}