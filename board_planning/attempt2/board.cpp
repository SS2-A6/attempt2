/*
 * board.cpp
 *
 *  Created on: 2016/12/21
 *      Author: mizuho
 */

#include <mbed.h>
#include "attempt2/board.h"

RGB::RGB(PinName r, PinName g, PinName b){
	R = new DigitalOut(r);
	G = new DigitalOut(g);
	B = new DigitalOut(b);
}

void RGB::black(){
	*R = 0;
	*G = 0;
	*B = 0;
}

void RGB::blue(){
	*R = 0;
	*G = 0;
	*B = 1;
}

void RGB::cyan(){
	*R = 0;
	*G = 1;
	*B = 1;
}

void RGB::green(){
	*R = 0;
	*G = 1;
	*B = 0;
}

void RGB::magenta(){
	*R = 1;
	*G = 0;
	*B = 1;
}

void RGB::red(){
	*R = 1;
	*G = 0;
	*B = 0;
}

void RGB::white(){
	*R = 1;
	*G = 1;
	*B = 1;
}

void RGB::yellow(){
	*R = 1;
	*G = 1;
	*B = 0;
}
