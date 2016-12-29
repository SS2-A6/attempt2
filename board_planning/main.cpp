#include <mbed.h>
#include "attempt2/board.h"

Serial pc(USBTX, USBRX);

#define rgb_

#ifdef rgb_
//DigitalOut myled(LED1);
//RGB led;
RGB led(PA_15, PC_12, PC_10, 0, 1);

int main() {
    DigitalOut myled(LED1);     // LED1 == PA_5
    int i = 0;

    while(1) {
        myled = 1; // LED is ON
        Thread::wait(500);

        i = 1;

        led.white();
        Thread::wait(500);
        led.red();
        Thread::wait(500);
        led.green();
        Thread::wait(500);
        led.blue();
        Thread::wait(500);
        led.magenta();
        Thread::wait(500);
        led.yellow();
        Thread::wait(500);
        led.cyan();
        Thread::wait(500);

        myled = 0; // LED is OFF
        Thread::wait(300); // 200 ms
        led.black();
        Thread::wait(300); // 200 ms
    }
}
#elif defined(servo)
PwmOut s1(PB_4);
PwmOut s2(PB_3);

int main(){

    s1.period_us(14000);
    s2.period_us(14000);
    while(1){
        s1.pulsewidth_us(600);
        s2.pulsewidth_us(600);
        Thread::wait(500);

        s1.pulsewidth_us(1400);
        s2.pulsewidth_us(1400);
        Thread::wait(500);

        s1.pulsewidth_us(2000);
        s2.pulsewidth_us(2000);
        Thread::wait(500);
    }
}
#elif defined(motor)
PwmOut m1(PA_5);
PwmOut m2(PA_6);
#define F 0.0001d
int main(){
    m1.period_ms(10);
    m2.period_ms(10);

    m1.pulsewidth_us(10000);
    m2.pulsewidth_us(10000);

    uint64_t t = 0;
    for(;;t++){
        m1.pulsewidth_us(5000 + (int)(5000*sin(2*M_PI*F*t)));
        m2.pulsewidth_us(5000 + (int)(5000*cos(2*M_PI*F*t)));
    }

}
#elif defined(magnet)
DigitalIn mag(PB_5);
DigitalOut led(PA_5);
int main(){
   while(1){
       led = mag;
   }
}
#endif


