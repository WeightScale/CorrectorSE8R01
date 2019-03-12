#include "CORE.h"
#include "HX711.h"
#include "ButtonsClass.h"

CoreClass CORE;

value_t EEMEM core_value_eep;
value_t core_value;

CAT5171 POT_PLUS(CAT5171_AD0);
CAT5171 POT_MINUS(CAT5171_AD1);

CoreClass::CoreClass(){	
};
	
CoreClass::~CoreClass(){};

void CoreClass::begin(){
	Wire.begin();
	Wire.setClock(400000);	
	eeprom_read_block (&core_value, &core_value_eep, sizeof(value_t));	
	/*if (core_value.factorO == 0){
		core_value.factorO = OFFSET_HX711;
	}*/
}	

/*! 
*	\brief Функция калибровки плюсового значения. 
*	
*/
void CoreClass::doCalibration(){
	unsigned char p0 = 0;					///< Для визуального определения что вошли в калибровку.
	CORE.reset();
	delay(100);	
	core_value.offset = hx711.read();
	core_value.l_adc = 0;
	for(p0 = 0; p0 < 255; p0++){
		POT_PLUS.setResistance(p0);
		delay(20);
		long a = hx711.read();
		if (a > core_value.l_adc){
			core_value.l_adc = a;
		}else if(a <= core_value.l_adc){
			p0++;
			POT_PLUS.setResistance(p0);
			a = hx711.read();
			if (a > core_value.l_adc)
				core_value.l_adc = a;
			else{
				core_value.corrMtoP = --p0;
				break;
			}				
		}else{
			core_value.corrMtoP = p0;
			break;	
		}
	}
	core_value.corrMtoP = p0;
	CORE.reset();	
	/*core_value.l_adc -= core_value.offset;
	float fp = 0,fm = 0, f = 0;
	for(int i=0; i<FACTOR_MAX; i++){
#if FACTOR_PLAN == FACTOR_5_10_15_20
		switch(i){
			case 0:
				fp=0.075;
				fm=-0.075;
			break;
			case 1:
				fp=0.165;
				fm=-0.165;
			break;
			case 2:
				fp=0.21;
				fm=-0.30;
			break;
			case 3:
				fp=0.26;
				fm=-0.35;
			break;
		}
#else if FACTOR_PLAN == FACTOR_1_3_5
		switch(i){
			case 0:
				fp=0.01;
				fm=-0.01;
			break;
			case 1:
				fp=0.035;
				fm=-0.040;
			break;
			case 2:
				fp=0.055;
				fm=-0.062;
			break;
		}
#endif		
		//fp+=0.05;
		//fm+=-0.075;
		//core_value.factorP[i] = float(float(core_value.corrMtoP)* f) / float(core_value.l_adc);
		core_value.factorP[i] = float(float(core_value.corrMtoP)* fp) / float(core_value.l_adc);
		core_value.factorM[i] = float(float(core_value.corrMtoP)* fm) / float(core_value.l_adc);
	}*/	
	eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
	while(1){
		switch (Buttons.getCommand(375)){
			case B_UP_delay:				///< Добатить вес.
				while(1){					
					switch(Buttons.getCommand(250)){
						case B_UP:				///< Добатить вес.
							POT_PLUS.setResistance(++p0);
						break;
						case B_DOWN:				///< Отнять вес.
							POT_PLUS.setResistance(--p0);
						break;
						case B_CENTER:
							int p = POT_PLUS.getResistance();
							core_value.r_adc = hx711.read();
							core_value.factorO = float(p) / float(core_value.r_adc - core_value.offset);
							core_value.corrMtoP = p0;
							eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
							POT_PLUS.setResistance(0);
							POT_PLUS.twitch(10, 1000);
							POT_PLUS.setResistance(p0);
							return;
						break;
					}
				}
			break;
			case B_DOWN_delay:				///< Отнять вес.
				while(1){					
					switch(Buttons.getCommand(250)){
						case B_UP:				///< Добатить вес.
							POT_MINUS.setResistance(--p0);
						break;
						case B_DOWN:				///< Отнять вес.
							POT_MINUS.setResistance(++p0);
						break;
						case B_CENTER:
							int p = (POT_MINUS.getResistance() * (-1));
							core_value.r_adc = hx711.read();
							core_value.factorO = float(p) / float(core_value.r_adc - core_value.offset);
							core_value.corrMtoP = p0;
							eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
							POT_MINUS.setResistance(0);
							POT_MINUS.twitch(10, 1000);
							POT_MINUS.setResistance(p0);
							return;
						break;
					}
				}
			break;
			case B_CENTER:				///< Калибровка наклона сдвига отклонения от реального веса.
				//p0 = POT_PLUS.getResistance();
				p0 = 0;
				core_value.r_adc = hx711.read();
				core_value.factorO = float(p0) / float(core_value.r_adc - core_value.offset);
				core_value.corrMtoP = p0;
				eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
				POT_PLUS.setResistance(0);
				POT_PLUS.twitch(10, 1000);
				POT_PLUS.setResistance(p0);
				goto calout;
			break;
			case B_RIGHT:				///< Выйти без сохранения.
				goto calout;
			break;	
		}
		
		/*if (remoteController.readBitsPortToTime(2000)){			
			switch(remoteController.getBits()){				
				case ACTION_BUTTON_A:				///< Добатить вес.
					POT_PLUS.setResistance(++p0);
				break;
				case ACTION_BUTTON_B:				///< Отнять вес.
					POT_PLUS.setResistance(--p0);
				break;
				case ACTION_BUTTON_C:	
				case OFFSET_CALIBRATION:			///< Калибровка наклона сдвига отклонения от реального веса.
					p0 = POT_PLUS.getResistance();
					core_value.r_adc = hx711.read();
					core_value.factorO = float(p0) / float(core_value.r_adc - core_value.offset);
					core_value.corrMtoP = p0;	
					eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
					POT_PLUS.setResistance(0);
					POT_PLUS.twitch(10, 1000);					
					POT_PLUS.setResistance(p0);	
					goto calout;
				break;
				case ACTION_BUTTON_D:				///< Выйти без сохранения.
					goto calout;
				break;
			}
		}*/
	}	
	calout:
	{
		POT_PLUS.setResistance(0);					///< Для визуального определения что вышли в корректировку плюсования.
		POT_PLUS.twitch(10, 1000);		
	}
}

/*void CoreClass::plusCalibration(uint8_t * p){
	while(1){
		if (remoteController.readBitsPortToTime(2000)){
			switch(remoteController.getBits()){
				case ACTION_BUTTON_A:				///< Добатить вес.
					POT_PLUS.setResistance(++p);
				break;
				case ACTION_BUTTON_B:				///< Отнять вес.
					POT_PLUS.setResistance(--p);
				break;
				case ACTION_BUTTON_C:
				break;				
			}	
		}
	}
}*/

	

void CoreClass::doPlus(){
	int i = 0;
	POT_PLUS.twitch(10, 2000);										///< Для визуального определения что вошли в корректировку юсования.
	POT_MINUS.setResistance(0);
	while(1){			
		//int res = float(hx711.read() - core_value.offset) * (core_value.factorP[i] + core_value.factorO);		///< Вычисляем значение сопротивления для корекции.
		int res = float(hx711.read() - core_value.offset) * core_value.factorP[i];
		//res = abs(res);
		//res = constrain(res, 0, 255);																///< Чтобы не вышло из диапазона.
		//POT_PLUS.setResistance(res);
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
			switch(Buttons.getCommand(500)){
				case B_UP:
					int t;
					if (++i >=FACTOR_MAX){
						i = 0;
					}
					t = (i+1) * 10;
					POT_PLUS.twitch(t, 2000);														///< Для визуального определения что вышли в корректировку плюсования.					
				break;
				case B_RIGHT_delay:
					{												
						unsigned char p = (char)res;
						bool f = true;
						POT_PLUS.twitch(10, 1000);
						//while(Buttons.isPressed()){};
						while(f){							
							switch(Buttons.getCommand(250)){
								case B_UP:
									POT_PLUS.setResistance(++p);
								break;
								case B_DOWN:
									POT_PLUS.setResistance(--p);
								break;
								case B_RIGHT:
									POT_PLUS.twitch(10, 1000);
									int pot = POT_PLUS.getResistance();
									core_value.r_adc = hx711.read();
									core_value.factorP[i] = float(pot) / float(core_value.r_adc - core_value.offset);
									eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
									POT_PLUS.setResistance(0);
									POT_PLUS.twitch(10, 1000);
									POT_PLUS.setResistance(pot);
									f = false;
								break;
							}
						}	
					}
				break;
				case B_CENTER:
					POT_PLUS.twitch(10, 500);														///< Для визуального определения что вышли в корректировку плюсования.
					POT_PLUS.reset();
					return;
				case B_DOWN:
					goto _minus;
			}			
	}
	_minus: ;
	{
		Buttons.setPressed(true);											///< Устанавливаем флаг чтобы кнопка минус сработала	
	}
}

void CoreClass::doMinus(){
	int i = 0;			
	POT_MINUS.twitch(10, 2000);														///< Для визуального определения что вошли в корректировку минусования.
	POT_PLUS.setResistance(0);
	while(1){
		//int res = float(hx711.read() - core_value.offset) * (core_value.factorM[i] + core_value.factorO);///< Вычисляем значение сопротивления для корекции.
		int res = float(hx711.read() - core_value.offset) * core_value.factorM[i];
		//res = abs(res);			
		//res = constrain(res, 0, 255);												///< Чтобы не вышло из диапазона.
		//POT_MINUS.setResistance(res);
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
		//if (remoteController.readBitsFromPort()){									///< Выходим из коректировки.
			switch(Buttons.getCommand(500)){
				case B_DOWN:
				int t;
					if (++i >=FACTOR_MAX){
						i = 0;
					}
					t = (i+1) * 10;
					POT_MINUS.twitch(t, 2000);										///< Для визуального определения что вышли в корректировку минусования.
				break;
				case B_RIGHT_delay:
					{
						unsigned char p = (char)res;
						bool f = true;
						POT_MINUS.twitch(10, 1000);
						//while(Buttons.isPressed()){};
						while(f){							
							switch(Buttons.getCommand(250)){
								case B_UP:
									POT_MINUS.setResistance(--p);
								break;
								case B_DOWN:
									POT_MINUS.setResistance(++p);
								break;
								case B_RIGHT:
									POT_MINUS.twitch(10, 1000);
									int pot = (POT_MINUS.getResistance()* (-1));
									core_value.r_adc = hx711.read();
									core_value.factorM[i] = float(pot) / float(core_value.r_adc - core_value.offset);
									eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
									POT_MINUS.setResistance(0);
									POT_MINUS.twitch(10, 1000);
									POT_MINUS.setResistance(pot);
									f = false;
								break;
							}							
						}	
					}		
				break;
				case B_CENTER:					
					POT_MINUS.twitch(10, 500);										///< Для визуального определения что вышли в корректировку минусования.
					POT_MINUS.reset();
				return;
				case B_UP:
					goto _plus;
			}			
		//}
	}
	_plus: ;
	{
		Buttons.setPressed(true);											///< Устанавливаем флаг чтобы кнопка плюс сработала	
	}
}

void CoreClass::standart(){
	hx711.powerDown();
	//hx711.powerUp();
	CORE.disconnect();
	while(1){		
		//if (remoteController.readBitsFromPort()){
			switch(Buttons.getCommand()){
				case B_CENTER:
				case B_UP:
				case B_DOWN:
					goto _exit;
				break;
			}			
		//}
	}
	{
		_exit: ;
		hx711.powerUp();
		Buttons.setPressed(true);
	}
}

void CoreClass::disconnect(){
	POT_PLUS.shutdown();
	POT_MINUS.shutdown();
}

void CoreClass::reset(){
	POT_PLUS.setResistance(0);
	POT_MINUS.setResistance(0);
}


	
