// ButtonClass.h

#ifndef _BUTTONCLASS_h
#define _BUTTONCLASS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define PLUS_CALIBRATION	12	///< ������ ��� ������ C and A
#define MINUS_CALIBRATION	9	///< ������ ��� ������ C and B
#define OFFSET_CALIBRATION	10	///< ������ ��� ������ C and D ����������� ���������� �� ��������� ����

#define ACTION_BUTTON_LEFT		23	///< ������ ������
#define ACTION_BUTTON_RIGHT		29	///< ������ ������
#define ACTION_BUTTON_UP		30	///< ������ ������
#define ACTION_BUTTON_DOWN		27	///< ������ ������
#define ACTION_BUTTON_CENTER	15	///< ������ ������

enum BUTTON{
	NONE,
	B_LEFT,
	B_RIGHT,
	B_UP,
	B_DOWN,
	B_CENTER,
	B_LEFT_delay,
	B_RIGHT_delay,
	B_UP_delay,
	B_DOWN_delay,
	B_CENTER_delay
};

class ButtonsClass{
protected:
	bool _isDelay;
	bool _isPress;
	byte _button;
	uint16_t _timePressed;

 public:
	void init(byte button, bool isPress,bool isDelay, uint16_t pressed_time);
	BUTTON getCommand(long t = 4000);
	bool isPressed(){return _isPress;};
	void setPressed(bool p){_isPress = p;};
	void clearPress(){_isPress = false;};
	uint16_t getTime(){return _timePressed;};
	byte getButton(){return _button;};			
};

extern ButtonsClass Buttons;

#endif

