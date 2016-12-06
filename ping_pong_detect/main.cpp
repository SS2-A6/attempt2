#include "mbed.h"
#include "attempt2.h"

Serial pc(USBTX, USBRX);
uint16_t ball_count = 0;

int main() {

	pc.baud(115200);

	Thread thread1;

	osStatus err = thread1.start(&ball_psd_read);
	if (err) {
		pc.printf("thread1 error!!\n");
	}

	for(;;){
		//pc.printf("%d\n", ball_count);
	}

}
