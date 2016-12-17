#include <mbed.h>
#include <math.h>

Serial pc(USBTX, USBRX);
PwmOut servo(PB_3);
float make_arctan ( int8_t from_degree, int8_t to_degree, uint16_t t, uint16_t max_time, float x_p, float y_p );
void moving_arm ( int8_t from_degree, int8_t to_degree, uint16_t max_time, float x_p, float y_p );


int main() {

	pc.baud(115200);  // ボーレートの設定
	servo.period(0.014);  // サーボの周期

	// サーボ実験データ (Futaba S3003, 5V)
	//-90 : servo.pulsewidth(0.00060);
	// 00 : servo.pulsewidth(0.00140);
	//+90 : servo.pulsewidth(0.00220);

	// テスト
	moving_arm( -60, 60, 3000, 25, 90);
	moving_arm( 60, -60, 3000, 25, 90);

}


// アーム移動関数
// 引数 ( スタート角度, ゴール角度, 移動目標時間, 中間滑らか度, 漸近度 )
// 中間滑らか度 (中間傾斜区間を全体の何%ほどにするか)
// 漸近度 (漸近区間を真値に何%ほど近づけるか．サーボ安全率も担保するため固定推奨)
void moving_arm ( int8_t from_degree, int8_t to_degree, uint16_t max_time, float x_p, float y_p ) {

	uint16_t t;  // 移動現在時刻 (ms)
	uint16_t delta_t = 5;  // 分割時間 (ms) (細かくすれば動き滑らか)
	for ( t=0; t<=max_time; t+=delta_t ) {
		servo.pulsewidth( make_arctan( from_degree, to_degree, t, max_time, x_p, y_p ) );
		Thread::wait( delta_t );
	}

}


// アークタンジェント自動生成関数
// 引数 ( スタート角度, ゴール角度, 移動現在時刻, 移動目標時間, 中間滑らか度, 漸近度 )
// 中間滑らか度 (中間傾斜区間を全体の何%ほどにするか)
// 漸近度 (漸近区間を真値に何%ほど近づけるか．サーボ安全率の担保のため固定推奨!)
float make_arctan ( int8_t from_degree, int8_t to_degree, uint16_t t, uint16_t max_time, float x_p, float y_p  ) {

	float c = 100/(max_time*x_p) * tan( M_PI*y_p/100 - M_PI/2  );  // x_p, y_pから横軸圧縮率cを自動計算
	float from_val = 1.0e-5 * from_degree + 1.4e-3;  // スタート角度からサーボ用数値へ変換
	float to_val = 1.0e-5 * to_degree + 1.4e-3;  // ゴール角度からサーボ用数値へ変換

	return (to_val - from_val)/M_PI * atan( c*(t - max_time/2.0) ) + (from_val + to_val)/2.0;

}
