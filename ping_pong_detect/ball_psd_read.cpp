#include "mbed.h"
#define ball_psd_offset 5000

extern Serial pc;
extern uint16_t ball_count;
extern Timer timer;
extern uint8_t supply_state;



int before_time = 0;
int move_arm_time = 2147483647;
int before_time_saving = 0;  // before_time を計算するための一時保存用変数．特に意味は無い．
uint8_t first_ball_flag = 1;  // 1：ピンポン球1つ目フラグ，0：ピンポン球2つ目以降フラグ
// ピンポン球の検出 & ステートの遷移を管理する関数
void ball_psd_read() {

	AnalogIn myPSD(PA_0);
	uint32_t ball_psd;
	uint32_t ball_psd_before = myPSD.read_u16();
	int32_t ball_psd_diff;

	while (true) {

		ball_psd = myPSD.read_u16();
		ball_psd_diff = ball_psd - ball_psd_before;
		ball_psd_before = ball_psd;
		pc.printf("%d, %d\n", ball_psd, ball_count);

		// ピンポン球が落ちてきたとき
		if ( ball_psd_diff < -ball_psd_offset ) {
			ball_count++;
			supply_state = 1;
			before_time = before_time_saving;  // before_time に前回ピンポン球取得時の時刻を記録する
			before_time_saving = timer.read_ms();  // before_time_saving には今回分の時刻を一時保存しておいて，次回の before_time に活用する
			Thread::wait(1000);
		}

		// 供給権を持っていると"思われる"とき
		if ( supply_state == 1 ) {
			// 供給権を確認し，紛失していたら supply_state=2 へ
			supply_me( timer.read_ms() );
		}
		// 供給権を紛失し，相手が保持しているとき
		else if ( supply_state == 2 ) {
			// 供給権を取り返すため，アームを動かす
			move_arm_out();
			move_arm_in();
			supply_state = 0;
			first_ball_flag = 1;  // 次出てくるピンポン球は第1球として扱うので注意
			move_arm_time = timer.read_ms();  // 最後にアームを動かした時刻を記録しておく
		}
		// どっちに供給権があるのか不明な，空白状態のとき
		else if ( supply_state == 0 ) {
			// 基本は何もしないでステイ．
			// もしも空白状態が長く続いているなら供給権紛失している可能性があるので，再び supply_state = 2 へ入る
			if ( (timer.read_ms()-move_arm_time) > 4000 ) {
				supply_state = 2;
			}
		}

	}
}



#define time_offset 500  // 規定時間をどれだけオーバーしたら，供給権紛失とみなすか
#define time_eq(now,before,offset) ( now - before + offset )

// 自分に供給権があると"思われる"ときの関数
// 1. before_time と now_time を比較して，本当に供給権を保持しているのか確認
// 2. 紛失していれば，supply_state=2 へ入る
// 3. 保持していれば，そのままステイ
// 4. 毎度，供給権獲得後のピンポン球1つ目は要注意
void supply_me ( int now_time ) {

	// 毎度，供給権獲得後のピンポン球1つ目は，とりあえずステイ
	if ( first_ball_flag == 1 ) {
		first_ball_flag = 0;  // フラグクリア
	}

	// 序盤戦 (00s-60s)
	else if ( now_time < 1000 * 60 ) {
		// 供給権紛失条件
		if ( time_eq(now_time,before_time,time_offset) > 2000+time_offset) {
			supply_state = 2;
		}
		// 落ち続けていれば，そのままステイ
		else {}
	}

	// 中盤戦 (60s-120s)
	else if ( (now_time >= 1000 * 60) && (now_time < 1000 * 120) ) {
		// 供給権紛失条件
		if ( time_eq(now_time,before_time,time_offset) > 1500+time_offset) {
			supply_state = 2;
		}
		// 落ち続けていれば，そのままステイ
		else {}
	}

	// 終盤戦 (120s-180s)
	else if ( (now_time >= 1000 * 120) && (now_time < 1000 * 180) ) {
		// 供給権紛失条件
		if ( time_eq(now_time,before_time,time_offset) > 1000+time_offset) {
			supply_state = 2;
		}
		// 落ち続けていれば，そのままステイ
		else {}
	}

}



// アームを動かして，供給権エリアに入る関数
void move_arm_in() {
}



// アームを動かして，供給権エリアから出る関数
void move_arm_out() {
}
