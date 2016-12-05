#include "mbed.h"
//#include "rtos.h"

/*
Semaphore slots(1);
int test = 0;

void some_work(){

	for (int i=0;i<100000;i++) { slots.wait(); test++; slots.release(); }

}

void some_work2(){

	for (int i=0;i<100000;i++) { test++; }

}
*/

AnalogIn myPSD(PA_0);
Serial pc(USBTX, USBRX);
PwmOut myLED(LED1);
#define offset 8000
int main() {

	uint16_t ball_psd;
	uint16_t ball_psd_before = 0;
	int16_t ball_diff;
	uint16_t ball_count = 0;
	uint16_t ball_flag = 0;

    while (true) {

        ball_psd = myPSD.read_u16();
        ball_diff = ball_psd - ball_psd_before;
        ball_psd_before = ball_psd;
        pc.printf("%d %d\n", ball_psd, ball_count);
        //myLED = val/65535.0f;
        //Thread::wait(30);

        if ( ball_flag == 0 ) {
        	if ( ball_diff < -offset ) {
        		ball_flag = 1;
        	}
        }
        else {
        	if ( ball_diff > offset ) {
        		ball_flag = 0;
        		ball_count ++;
        		Thread::wait(10);
        		//pc.printf("%d\n", ball_count);
        	}
        }

    }

}

