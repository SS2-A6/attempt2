#include "mbed.h"
#include "attempt2.h"

extern Serial pc;
extern uint16_t ball_count;
extern Timer timer;
extern uint8_t supply_state;
extern Mutex mutex;

#define ball_psd_offset 5000
#define rand_psd_max 5800  // 実験用psd制限
uint32_t ball_psd = 0;
int32_t ball_psd_diff = 0;
uint32_t ball_psd_before = 0;
int before_time = 0;
uint8_t first_ball_flag = 1;  // 1：ピンポン球1つ目フラグ，0：ピンポン球2つ目以降フラグ
// ピンポン球落下の検出を行う関数
void ball_psd_read() {

	AnalogIn myPSD(PA_0);
	mutex.lock();
	ball_psd = myPSD.read_u16() % rand_psd_max;  // 実験用
	ball_psd_before = myPSD.read_u16() % rand_psd_max;  // 実験用
	mutex.unlock();

	while (true) {

		mutex.lock();
		// ball_psd = myPSD.read_u16();
		ball_psd = rand() % rand_psd_max;  // 実験用
		ball_psd_diff = ball_psd - ball_psd_before;
		ball_psd_before = ball_psd;
		mutex.unlock();

		// ピンポン球が落ちてきたとき
		if (ball_psd_diff < -ball_psd_offset) {
			ball_count++;
			supply_state = 1;
			before_time = timer.read_ms();
			Thread::wait(1000);
		}

	}

}
