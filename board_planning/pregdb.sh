#!/bin/bash

openocd -s ~/sources/OpenOCD/ARMEclipseGNU/openocd/0.10.0-201610281609-dev/scripts/ -f board/st_nucleo_f4.cfg &
cd /home/mizuho/sources/SS2/attempt2/board_planning
arm-none-eabi-gdb -x pregdb.conf
