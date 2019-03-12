// CORE.h

#ifndef _CORE_h
#define _CORE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <avr/eeprom.h>
#include "CAT5171.h"

#define FACTOR_MAX 4
#define OFFSET_HX711 0.000031

#define FACTOR_5_10_15_20			10
#define FACTOR_1_3_5				20

#define FACTOR_PLAN					FACTOR_1_3_5
//#define FACTOR_PLAN					FACTOR_5_10_15_20

#if FACTOR_PLAN == FACTOR_1_3_5
	#define FACTOR_MAX 3
#else if FACTOR_PLAN == FACTOR_5_10_15_20
	#define FACTOR_MAX 4
#endif

typedef struct{
	long offset  ;					//< код ацп нулевой вес
	long l_adc;
	long r_adc;						//< значение ацп реального веса
	float factorO;					//< коэфоциент наклон отклонения 
	float factorP[FACTOR_MAX];		//< коэфициент перевода ацп в сопротивление для прибавления
	float factorM[FACTOR_MAX];		//< коэфициент перевода ацп в сопротивление для вычитания
	unsigned char corrPtoM;			//< для коректировки плюса с помощью минуса запоминаем сопротивление минуса
	unsigned char corrMtoP;			//< для коректировки минуса с помощью плюса запоминаем сопротивление минуса
}value_t;

extern value_t EEMEM core_value_eep;
extern value_t core_value;

 /*! \brief Основной класс для работы с коректором.
 
	
 */
class CoreClass{
	
	private:
		long _adc_ofset;		
	
	public:		
		CoreClass();
		~CoreClass();
		void begin();
		/*! \brief Функция калибровки плюсового значения*/
		void doCalibration();
		//void plusCalibration(uint8_t * p);
		//void minusCalibration();		
		void doPlus();
		void doMinus();
		void standart();
		void disconnect();
		void reset();
		void setAdcOfset(long a){_adc_ofset = a;};
		long getAdcOfset(){return _adc_ofset;}	
};

extern CoreClass CORE;
extern CAT5171 POT_PLUS;
extern CAT5171 POT_MINUS;

#endif

