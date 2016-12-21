#include "mbed.h"
#include "attempt2.h"

extern Serial pc;
extern uint16_t ball_count;
extern Timer timer;
extern uint8_t supply_state;
extern uint8_t first_ball_flag;
extern int before_time;


int move_arm_time = 2147483647;  // 最後にアームを動かした時刻を記録しておく
#define rescue_time 10000  // 空白状態での供給権紛失を回避するための制限時間
// ステートごとに動作を行う関数
void state_work() {

	while (true) {

		// 供給権を持っていると"思われる"とき
		if (supply_state == 1) {
			// 供給権を確認し，紛失していたら supply_state=2 へ
			supply_me( timer.read_ms() );
		}

		// 供給権を紛失し，相手が保持しているとき
		else if (supply_state == 2) {
			pc.printf("*** move arm!!\n");
			// 供給権を取り返すため，アームフラグを立て，B機に動かしてもらう
			move_arm_flag = 1;
			first_ball_flag = 1;  // 次出てくるピンポン球は第1球として扱うので注意
			// アームフラグをxbeeで読み，降ろされるまで次に進まない
			while ( xbee.read(move_arm_flag) == 1 ) {
			}
			supply_state = 0;
			move_arm_time = timer.read_ms();  // 最後にアームを動かした時刻を記録しておく
		}

		// どっちに供給権があるのか不明な，空白状態のとき
		else if (supply_state == 0) {
			// 基本は何もしないでステイ．
			// もしも空白状態が長く続いているなら供給権紛失している可能性があるので，再び supply_state = 2 へ入って危機回避
			if ( (timer.read_ms() - move_arm_time) > rescue_time ) {
				supply_state = 2;
			}
		}

	}

}



#define time_offset 1000  // 規定時間をどれだけオーバーしたら，供給権紛失とみなすか
#define time_eq(t1,t2,err) ( (t2 - err <= t1) && (t1 <= t2 + err) )   // t1とt2を比較して，errの範囲内で一致判定を返す関数
#define time_err 100  // 供給装置の誤差予想

// 自分に供給権があると"思われる"ときの関数
// 1. before_time と now_time を比較して，本当に供給権を保持しているのか確認
// 2. 紛失していれば，supply_state=2 へ入る
// 3. 保持していれば，そのままステイ
// 4. 毎度，供給権獲得後のピンポン球1つ目は要注意
void supply_me( int now_time ) {

	// 毎度，供給権獲得後のピンポン球1つ目は，とりあえずステイ
	if (first_ball_flag == 1) {
		first_ball_flag = 0;  // フラグクリア
	}

	// 序盤戦 (00s-60s)
	else if (now_time < 1000 * 60) {
		// 供給権紛失条件
		if (time_eq(now_time - before_time, 2000+time_offset, time_err)) {
			supply_state = 2;
		}
		// 落ち続けていれば，そのままステイ
		else {
		}
	}

	// 中盤戦 (60s-120s)
	else if ((now_time >= 1000 * 60) && (now_time < 1000 * 120)) {
		// 供給権紛失条件
		if (time_eq(now_time - before_time, 1500+time_offset, time_err)) {
			supply_state = 2;
		}
		// 落ち続けていれば，そのままステイ
		else {
		}
	}

	// 終盤戦 (120s-180s)
	else if ((now_time >= 1000 * 120) && (now_time < 1000 * 180)) {
		// 供給権紛失条件
		if (time_eq(now_time - before_time, 1000+time_offset, time_err)) {
			supply_state = 2;
		}
		// 落ち続けていれば，そのままステイ
		else {
		}
	}

}
