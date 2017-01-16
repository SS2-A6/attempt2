#include <mbed.h>
#include <attempt2.h>
#include "attempt2/board.h"
// A機：静止してボール検知 & ステート管理

extern int before_time;
extern uint32_t ball_psd;
extern int32_t ball_psd_diff;
extern int move_arm_time;

// プロトタイプ宣言
void move_arm ( int8_t from_degree1, int8_t to_degree1, int8_t from_degree2, int8_t to_degree2, uint16_t move_arm_time, float x_p, float y_p );
float make_arctan ( int8_t from_degree, int8_t to_degree, uint16_t t, uint16_t max_time, float x_p, float y_p );

Serial pc(PA_9, PA_10);  // Xbee_A
Serial debug(USBTX, USBRX);  // デバッグ用print
PwmOut servo1(PB_3);  // 栓抜き用サーボ
PwmOut servo2(PB_4);  // 未使用サーボ
Timer timer;
uint16_t ball_count = 0;
uint8_t ready_flag_A = 0;
uint8_t ready_flag_B = 0;
uint8_t supply_state = 0;
uint8_t move_arm_flag = 0;
Mutex mutex;


// Xbee_Aが受信したら実行する関数
RGB led(PA_15, PC_12, PC_10, 0, 1);
void callback(){
	char var = pc.getc();
	debug.putc(var);
	led.red();
	Thread::wait(100);
	led.black();
    if( var<10 ) {
    	// 0:空白状態, 1:こちらが供給権, 2:相手が供給権
    	supply_state = var;
    }
    else if( (var>=10)&&(var<100) ) {
    	// 10:A未完, 11:A完了, 20:B未完, 21:B完了
    	ready_flag_B = var;
    }
    else if( (var>=100)&&(var<1000) ) {
    	// 100:フラグOFF, 101:フラグON
    	move_arm_flag = var;
    }
}


// A機メイン関数
int main() {

	pc.baud(9600);
	debug.bau5d(115200);
	debug.printf("Hello World!\n");
	Thread thread1;  // ball_psd_read() を回すスレッド
	Thread thread2;  // state_work() を回すスレッド
	pc.attach( &callback );  // Xbeeが受信したらcallback関数を実行

	// 戦闘準備 (サーボでブリッジをかける)
	move_arm(0, 60, 0, 0, 2000, 30, 90);  // サーボ1を用いて栓抜きを行う
	ready_flag_A = 11;  // A機は準備OK

	// A機・B機両方の準備が整うまで待機
	while ( !(ready_flag_A==11) || !(ready_flag_B==21) ) {

	}

	// A機戦闘開始
	timer.start();
	move_arm_time = timer.read_ms();  // B機側では戦闘開始時にアームを動かすので，ここでアーム時間記録
	// スレッド1の起動 & エラーの検出
	osStatus thread1_err = thread1.start( &ball_psd_read );
	if (thread1_err) {
		debug.printf("thread1 error!!\n");
	}
	// スレッド2の起動 & エラーの検出
	osStatus thread2_err = thread2.start( &state_work );
	if (thread2_err) {
		debug.printf("thread2 error!!\n");
	}

	// メインループ(2つのスレッド動作中)
	for(;;){

		/* 実験用
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
		*/

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
