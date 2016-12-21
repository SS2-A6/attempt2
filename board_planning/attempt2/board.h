/*
 * board.h
 *
 *  Created on: 2016/12/21
 *      Author: mizuho
 */
#include <mbed.h>

#ifndef ATTEMPT2_BOARD_H_
#define ATTEMPT2_BOARD_H_

/**
 * RGB LED. Default pin assign: R:PA_15, G:PC_10, B:PC_12
 * black, white, red, blue, green, cyan, yellow, magenta could be used
 */
class RGB{
public:
	RGB(PinName r = PA_15, PinName g = PC_10, PinName b = PC_12);
	void black();
	void white();
	void red();
	void blue();
	void green();
	void cyan();
	void yellow();
	void magenta();

	DigitalOut *R, *G, *B;
};


#endif /* ATTEMPT2_BOARD_H_ */
