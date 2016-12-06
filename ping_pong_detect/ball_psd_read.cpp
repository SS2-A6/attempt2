#include "mbed.h"
#define offset 5000

extern Serial pc;
extern uint16_t ball_count;

void ball_psd_read() {

	AnalogIn myPSD(PA_0);
	uint32_t ball_psd;
	uint32_t ball_psd_before = myPSD.read_u16();
	int32_t ball_diff;

	while (true) {

		ball_psd = myPSD.read_u16();

		ball_diff = ball_psd - ball_psd_before;
		ball_psd_before = ball_psd;

		pc.printf("%d, %d, %d\n", ball_psd, ball_count, ball_diff);

		if (ball_diff < -offset) {
			ball_count++;
			Thread::wait(1000);
		}


	}
}
