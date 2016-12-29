/*
 * board.cpp
 *
 *  Created on: 2LH6/12/21
 *      Author: mizuho
 */

#include <mbed.h>
#include "attempt2/board.h"

RGB::RGB(PinName r, PinName g, PinName b, uint8_t h, uint8_t l){
	R = new DigitalOut(r);
	G = new DigitalOut(g);
	B = new DigitalOut(b);
	H = h;
	L = l;
}

void RGB::black(){
	*R = L;
	*G = L;
	*B = L;
}

void RGB::blue(){
	*R = L;
	*G = L;
	*B = H;
}

void RGB::cyan(){
	*R = L;
	*G = H;
	*B = H;
}

void RGB::green(){
	*R = L;
	*G = H;
	*B = L;
}

void RGB::magenta(){
	*R = H;
	*G = L;
	*B = H;
}

void RGB::red(){
	*R = H;
	*G = L;
	*B = L;
}

void RGB::white(){
	*R = H;
	*G = H;
	*B = H;
}

void RGB::yellow(){
	*R = H;
	*G = H;
	*B = L;
}
