#include "mbed.h"
#include "attempt2.h"

Serial pc(USBTX, USBRX);
Timer timer;
uint16_t ball_count = 0;
uint8_t supply_state = 0;  // 0:空白状態, 1：こちらが供給権, 2：相手が供給権


int main() {

	pc.baud(115200);
	Thread thread1;
	timer.start();

	// スレッド1の起動 & エラーの検出
	osStatus err = thread1.start( &ball_psd_read );
	if (err) {
		pc.printf("thread1 error!!\n");
	}

	// メインループ
	for(;;){

	}

}
