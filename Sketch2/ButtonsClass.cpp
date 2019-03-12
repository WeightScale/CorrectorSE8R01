#include "ButtonsClass.h"

void ButtonsClass::init(byte button, bool isPress, bool isDelay, uint16_t pressed_time){
	_button = button;
	_isPress = isPress;
	_isDelay = isDelay;
	_timePressed = pressed_time;
}

BUTTON ButtonsClass::getCommand(long t){	
	if(isPressed()){
		clearPress();
		if(_isDelay){
			if (_timePressed > t){
				switch(_button){
					case ACTION_BUTTON_LEFT:
						return B_LEFT_delay;
					case ACTION_BUTTON_RIGHT:
						return B_RIGHT_delay;
					case ACTION_BUTTON_UP:
						return B_UP_delay;
					case ACTION_BUTTON_DOWN:
						return B_DOWN_delay;
					case ACTION_BUTTON_CENTER:
						return B_CENTER_delay;
					default:
						return NONE;
				}
			}	
		}else{			
			switch(_button){
				case ACTION_BUTTON_LEFT:
					return B_LEFT;
				case ACTION_BUTTON_RIGHT:
					return B_RIGHT;
				case ACTION_BUTTON_UP:
					return B_UP;
				case ACTION_BUTTON_DOWN:
					return B_DOWN;
				case ACTION_BUTTON_CENTER:
					return B_CENTER;
				default:
					return NONE;
			}
		}
	}
}


ButtonsClass Buttons;

