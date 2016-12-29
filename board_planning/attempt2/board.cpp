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
    logic = (h << 1) | l;
    black();
}

void RGB::black(){
    rgb(0,0,0);
}

void RGB::blue(){
    rgb(0,0,1);
}

void RGB::cyan(){
    rgb(0,1,1);
}

void RGB::green(){
    rgb(0,1,0);
}

void RGB::magenta(){
    rgb(1,0,1);
}

void RGB::red(){
    rgb(1,0,0);
}

void RGB::white(){
    rgb(1,1,1);
}

void RGB::yellow(){
    rgb(1, 1, 0);
}

inline void RGB::rgb(uint8_t r, uint8_t g, uint8_t b) {
    *R = (logic >> r) & 1;
    *G = (logic >> g) & 1;
    *B = (logic >> b) & 1;
}
