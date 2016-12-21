#include "mbed.h"
#include "attempt2.h"

extern int before_time;
extern uint32_t ball_psd;
extern int32_t ball_psd_diff;

Serial pc(USBTX, USBRX);
Timer timer;
uint16_t ball_count = 0;
uint8_t supply_state = 0;  // 0:空白状態, 1：こちらが供給権, 2：相手が供給権

Mutex mutex;

// A機メイン関数
int main() {

	pc.baud(115200);
	Thread thread1;  // ball_psd_read() を回すスレッド
	Thread thread2;  // state_work() を回すスレッド

	// 戦闘準備 (サーボでブリッジをかける)
	move_arm(????);
	ready_flag_A = 1;  // A機は準備OK

	// A機・B機両方の準備が整うまで待機
	while ( ( !xbee.read(ready_flag_B) )||( !ready_flag_A ) ) {

	}

	// A機戦闘開始
	timer.start();
	move_arm_time = timer.read_ms();  // B機側では戦闘開始時にアームを動かすので，ここでアーム時間記録
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

	for(;;){

		// 実験用
		mutex.lock();
		pc.printf("state=%d, ball_count=%d, psd=%lu, psd_diff=%ld, before_time=%d, now_time=%d, time_over=%d \n",
				supply_state,
				ball_count,
				ball_psd,
				ball_psd_diff,
				before_time,
				timer.read_ms(),
				timer.read_ms()-before_time);
		mutex.unlock();
		//

	}

}
