#include <mbed.h>
#include "attempt2/board.h"

Serial pc(USBTX, USBRX);
//PwmOut servo(PB_3);
//DigitalIn mag(PB_5);
//PwmOut motor1(PA_5);
//PwmOut motor2(PA_6);


#define RGB_Test

#ifdef RGB_Test
DigitalOut myled(LED1);
RGB led;

int main() {
    while(1) {
        myled = 1; // LED is ON
        Thread::wait(500);

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
    return 0;
}
#endif
