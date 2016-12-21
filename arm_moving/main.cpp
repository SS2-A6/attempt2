#include <mbed.h>
#include <math.h>

Serial pc(USBTX, USBRX);
Serial xbee();
PwmOut servo1(PB_3);  // 中央の超音波センサ用アーム
PwmOut servo2(PB_4);  // 手前の超音波センサ用アーム
DigitalIn mag(PB_5);
PwmOut motor1(PA_5);
PwmOut motor2(PA_6);

float make_arctan ( int8_t from_degree, int8_t to_degree, uint16_t t, uint16_t max_time, float x_p, float y_p );

void moving ( uint8_t move_mode, uint8_t time_mode, uint16_t move_time );

// 通信したい値
// supply_state, ready_flag_A, ready_flag_B, move_arm_flag

int main() {

	pc.baud(115200);  // ボーレートの設定
	servo1.period(0.014);  // サーボの周期
	servo2.period(0.014);  // サーボの周期

	// 戦闘準備 (開始後の定位置へのレール移動)
	while ( !mag.read() ) {
		move(2, 0, 0);
	}
	move(1, 1, 2000);  // 戦闘位置まで後進
	ready_flag_B = 1;  // B機は準備OK

	// A機側の準備が整うまで待機
	while ( ( !xbee.read(ready_flag_A) )&&( !read_flag_B ) ) {

	}

	// 戦闘開始
	move_arm(-90, 0, -90, -60, 3000, 30, 90);  // 初回の供給権確保へ

	while ( true ) {

		// ステートが2で，かつ，アームフラグ=1が立てられたら，アームを動かして供給権取り返す
		if ( ( xbee.read(supply_state) == 2 )&&( xbee.read(move_arm_flag) == 1 ) ) {
			move_arm_flag = 0;  // アームフラグクリア(送信)
			move_arm(0, -60, -60, 0, 2000, 30, 90);
			move_arm(-60, 0, 0, -60, 2000, 30, 90);
		}
		// それ以外のステートならば，B機はステイする
		else {
		}

	}

}


// テスト用関数
void debug_test () {

	// アーム移動テスト
	//move_arm( 0, -45, 0, -45, 3000, 30, 90);
	//move_arm( -45, 0, -45, 0, 3000, 30, 90);

	/*
	// マグネットテスト
	while ( true ) {
		uint8_t mag_val = mag.read();
		//pc.printf("%d\n", mag_val);

		if ( mag_val == 0 ) {
			move(1,0,0);
		}
		else if ( mag_val == 1 ) {
			move(0,0,0);
			break;
		}
	}
	*/

	// レール移動テスト
	//move(1, 1, 3000);
	//move(2, 1, 3000);

}


// レール移動関数
// 引数 ( 移動モード, 時間モード, 移動時間 )
// 移動モード 0：停止，1：前進，2：後進
// 時間モード 0：時間指定しない，1：時間指定する
// 移動時間 (ms)
void move ( uint8_t move_mode, uint8_t time_mode, uint16_t move_time ) {

	if ( move_mode == 0 ) {
		motor1.pulsewidth(0.0);
		motor2.pulsewidth(0.0);
		if ( time_mode == 1 ) {
			Thread::wait(move_time);
		}
	}
	else if ( move_mode == 1 ) {
		motor1.pulsewidth(0.0);
		motor2.pulsewidth(1.0);
		if ( time_mode == 1 ) {
			Thread::wait(move_time);
			motor1.pulsewidth(0.0);
			motor2.pulsewidth(0.0);
		}
	}
	else if ( move_mode == 2 ) {
		motor1.pulsewidth(1.0);
		motor2.pulsewidth(0.0);
		if ( time_mode == 1 ) {
			Thread::wait(move_time);
			motor1.pulsewidth(0.0);
			motor2.pulsewidth(0.0);
		}
	}

}


// アーム移動関数
// 引数 ( スタート角度1, ゴール角度1, スタート角度2, ゴール角度2, アーム移動時間, 中間滑らか度, 漸近度 )
// 各角度の番号はサーボ番号を表す
// 中間滑らか度 (中間傾斜区間を全体の何%ほどにするか)
// 漸近度 (漸近区間を真値に何%ほど近づけるか．サーボ安全率も担保するため固定推奨)
void move_arm ( int8_t from_degree1, int8_t to_degree1, int8_t from_degree2, int8_t to_degree2, uint16_t move_arm_time, float x_p, float y_p ) {

	uint16_t t;  // 移動現在時刻 (ms)
	uint16_t delta_t = 5;  // 分割時間 (ms) (細かくすれば動き滑らか)
	for ( t=0; t<=move_arm_time; t+=delta_t ) {
		servo1.pulsewidth( make_arctan( from_degree1, to_degree1, t, move_arm_time, x_p, y_p ) );
		servo2.pulsewidth( make_arctan( from_degree2, to_degree2, t, move_arm_time, x_p, y_p ) );
		Thread::wait( delta_t );
	}

}


// アークタンジェント自動生成関数
// 引数 ( スタート角度, ゴール角度, 移動現在時刻, 移動完了時間, 中間滑らか度, 漸近度 )
// 中間滑らか度 (中間傾斜区間を全体の何%ほどにするか)
// 漸近度 (漸近区間を真値に何%ほど近づけるか．サーボ安全率の担保のため固定推奨!)
float make_arctan ( int8_t from_degree, int8_t to_degree, uint16_t t, uint16_t max_time, float x_p, float y_p  ) {

	float c = 100/(max_time*x_p) * tan( M_PI*y_p/100 - M_PI/2  );  // x_p, y_pからグラフ圧縮率cを計算
	float from_val = 1.0e-5 * from_degree + 1.4e-3;  // スタート角度からサーボ用数値へ変換
	float to_val = 1.0e-5 * to_degree + 1.4e-3;  // ゴール角度からサーボ用数値へ変換

	return (to_val - from_val)/M_PI * atan( c*(t - max_time/2.0) ) + (from_val + to_val)/2.0;

}


///////////////////////////////////
// サーボ実験データ (Futaba S3003, 5V)
//-90 : servo.pulsewidth(0.00060);
// 00 : servo.pulsewidth(0.00140);
//+90 : servo.pulsewidth(0.00220);
///////////////////////////////////
