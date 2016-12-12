#include "mbed.h"
#include "attempt2.h"

extern int before_time;

Serial pc(USBTX, USBRX);
Timer timer;
uint16_t ball_count = 0;
uint8_t supply_state = 0;  // 0:空白状態, 1：こちらが供給権, 2：相手が供給権


int main() {

	pc.baud(115200);
	Thread thread1;  // ball_psd_read() を回すスレッド
	Thread thread2;  // state_work() を回すスレッド
	timer.start();

	// スレッド1の起動 & エラーの検出
	osStatus thread1_err = thread1.start( &ball_psd_read );
	if (thread1_err) {
		pc.printf("thread1 error!!\n");
	}

	// スレッド2の起動 & エラーの検出
	osStatus thread2_err = thread2.start( &state_work );
	if (thread2_err) {
		pc.printf("thread2 error!!\n");
	}

	// メインループ
	for(;;){

		// 実験用
		pc.printf("state=%d, ball_count=%d, before_time=%d, now_time=%d, time_over=%d \n", supply_state, ball_count, before_time, timer.read_ms(), timer.read_ms()-before_time);
		//

	}

}
