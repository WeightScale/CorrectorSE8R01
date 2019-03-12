//this is a copy and paste job made by F2k
#include "Arduino.h"
#include "RadioClass.h"
#include "ButtonsClass.h"
#include "CORE.h"
#include "HX711.h"


//***************************************************
void setup(){
	ACSR = (1<<ACD); //Turn off Analog Comparator - this removes about 1uA
	ADCSRA = 0x00;//Turn off ADC
	//PRR = 0xCF;		//Закоментировать перед использованием debugWire
	//PRR = 0xCB;	//Запрограммировать перед использованием debugWire
	
	SE8R01.init();
	SE8R01.onCallBack([](uint8_t *data, size_t len){
		uint16_t time = (data[4]<<8)|(data[5]);
		Buttons.init(data[1], true, data[2], time);
	});	
	CORE.begin();
	//CORE.standart();	
	hx711.powerUp();
	CORE.reset();
	core_value.offset = hx711.read();	
		
}

void loop(){
	/*if(digitalRead(IRQq)==LOW){
		SE8R01.parceData();
	}*/
	switch (Buttons.getCommand(500)){
		case B_CENTER_delay:			///< Войти в процесс калибровки плюсовой коррекции.
			CORE.doCalibration();
		break;
		case B_UP:			///< Включить додавление процентов.
			CORE.doPlus();
		break;
		case B_DOWN:			///< Включить снятие процениов.
			CORE.doMinus();
		break;
		case B_CENTER:			///< Сбросить коррекцию
			POT_PLUS.twitch(10, 1000);
			hx711.powerUp();
			CORE.reset();
		break;
		case B_RIGHT:			///< Отключится от схемы
			POT_PLUS.twitch(10, 1000);
			CORE.standart();
		break;
		case B_RIGHT_delay:
			POT_PLUS.twitch(10, 1000);
			CORE.reset();
			core_value.offset = hx711.read();
			POT_PLUS.setResistance(0xFF);
			while(1){
				switch(Buttons.getCommand()){
					case B_CENTER:
					case B_UP:
					case B_DOWN:
						return;
				}
			}
		break;
	}
	
	int res = float(hx711.read() - core_value.offset) * core_value.factorO;//0.00003032051;//0.00002062979;//core_value.factorO;
	//core_value.r_adc = hx711.read();
	//int res = float(core_value.r_adc - core_value.offset) / core_value.factorO;
	if (res < 0){
		res = abs(res);
		int r = constrain(res, 0, 255);
		POT_PLUS.setResistance(0);
		POT_MINUS.setResistance(r);
		}else{
		int r = constrain(res, 0, 255);
		POT_PLUS.setResistance(r);
		POT_MINUS.setResistance(0);
	}
	
	//delay(10);
}
