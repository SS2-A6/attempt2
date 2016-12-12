#include "mbed.h"
#include "attempt2.h"

extern Serial pc;
extern uint16_t ball_count;
extern Timer timer;
extern uint8_t supply_state;


#define ball_psd_offset 5000
int before_time = 0;
int before_time_saving = 0;  // before_time を計算するための一時保存用変数．特に意味は無い．
uint8_t first_ball_flag = 1;  // 1：ピンポン球1つ目フラグ，0：ピンポン球2つ目以降フラグ
// ピンポン球落下の検出を行う関数
void ball_psd_read() {

	AnalogIn myPSD(PA_0);
	uint32_t ball_psd_before = myPSD.read_u16();
	uint32_t ball_psd;
	int32_t ball_psd_diff;

	while (true) {

		// ball_psd = myPSD.read_u16();
		ball_psd = rand() % 5005;  // 実験用
		ball_psd_diff = ball_psd - ball_psd_before;
		ball_psd_before = ball_psd;


		// ピンポン球が落ちてきたとき
		if (ball_psd_diff < -ball_psd_offset) {
			ball_count++;
			supply_state = 1;
			before_time = before_time_saving; // before_time に前回ピンポン球取得時の時刻を記録する
			before_time_saving = timer.read_ms(); // before_time_saving には今回分の時刻を一時保存しておいて，次回の before_time に活用する
			Thread::wait(1000);
		}

	}

}
