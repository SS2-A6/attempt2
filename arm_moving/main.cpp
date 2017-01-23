#include <mbed.h>
#include <math.h>
#include "attempt2/board.h"
// B機：情報を受信してアームを動かし，供給権奪取

RGB led;
Serial pc(PA_9, PA_10);  // Xbee_B
Serial debug(USBTX, USBRX);  // デバッグ用
PwmOut servo1(PB_3);  // 中央の超音波センサ用アーム (適正角度 : 0 ~ -75)
PwmOut servo2(PB_4);  // 手前の超音波センサ用アーム (適正角度 : 0 ~ +85)
DigitalIn mag(PB_5);  // マグネットセンサ
PwmOut motor1(PA_5);  // 移動用モータ
PwmOut motor2(PA_6);  // 移動用モータ
uint8_t ready_flag_A = 0;
uint8_t ready_flag_B = 0;
uint8_t supply_state = 0;
uint8_t move_arm_flag = 0;

// プロトタイプ宣言
float make_arctan ( int8_t from_degree, int8_t to_degree, uint16_t t, uint16_t max_time, float x_p, float y_p );
void move ( uint8_t move_mode, uint8_t time_mode, uint16_t move_time );
void move_arm ( int8_t from_degree1, int8_t to_degree1, int8_t from_degree2, int8_t to_degree2, uint16_t move_arm_time, float x_p, float y_p );


// Xbee_Bが受信したら実行する関数
void callback(){

	char var = pc.getc();
    if( var<10 ) {
    	// 0:空白状態, 1:こちらが供給権, 2:相手が供給権
    	supply_state = var;
    }
    else if( (var>=10)&&(var<100) ) {
    	// 10:A未完, 11:A完了, 20:B未完, 21:B完了
    	ready_flag_A = var;
    }
    else if( (var>=100)&&(var<1000) ) {
    	// 100:フラグOFF, 101:フラグON
    	move_arm_flag = var;
    }

}


// B機メイン関数
int main() {

	uint8_t magnet_count = 0;
	led.red();
	debug.baud(115200);
	pc.baud(9600);  // ボーレートの設定
	servo1.period(0.014);  // サーボの周期
	servo2.period(0.014);  // サーボの周期
	pc.attach( &callback );  // Xbeeが受信したらcallback関数を実行

	debug.printf("\n\rStart.\n\r");
	move_arm(0,0,0,0, 2000, 30, 90);

	DigitalIn button(USER_BUTTON);
	while(button);
	led.yellow();

	// 戦闘準備 (開始後の定位置へのレール移動)
	//while (1){ debug.printf("%d\n\r", mag.read()); }
	while ( 1 ) {
		move(2, 0, 0);
		int magread = mag.read();
		debug.printf("%d count=%d\n\r", mag.read(), magnet_count);
		if (magread == 0){
			magnet_count++;
			if (magnet_count == 2){
				debug.printf("End magnet loop\n\r");
				break;
			}
			Thread::wait(2000);
		}
	}
	debug.printf("mag read OK.\n\r");

	move(1, 1, 2000);  // 戦闘位置まで後進
	debug.printf("move set OK.\n\r");

	ready_flag_B = 21;  // B機は準備OK
	debug.printf("B is OK.  Waiting for A...\n\r");
	led.green();

	// A機・B機両方の準備が整うまで待機
	//while ( !((ready_flag_A==11)&&(ready_flag_B==21)) ) {
		pc.putc(21);
		Thread::wait(100);
	//}

	debug.printf("A&B are OK. Misson Start!\n\r");


	// B機戦闘開始


	// 最初のアーム振り下ろしへ (A機はスレッド起動中なので少し時間かけてもよい)
	move_arm(0, -75, 0, 45, 3500, 30, 90);

	// メインループ
	while ( true ) {

		// ステートが2で，かつ，アームフラグ=1が立てられたら，アームを動かして供給権取り返す
		if ( ( move_arm_flag == 101 ) ) {
			pc.putc(100);  // アームフラグクリア&送信
			move_arm(-75, -45, 45, 85, 2000, 30, 90);
			move_arm(-45, -75, 85, 45, 2000, 30, 90);
			move_arm_flag = 100; // clear
			pc.putc(move_arm_flag);
		}
		// それ以外のステートならば，B機はステイする
		else {
		}

	}

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
