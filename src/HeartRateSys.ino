#include <Arduino.h>
#include <stdio.h>

#include "seg_disp.h"
#include "disp_io.h"
#include "stdio.h"
#include "hr_monitor.hpp"


typedef SegmentDisplay<4u> SegDisp;

#define SERIAL_PIN 4
#define SERIAL_CLK 6
#define OUTSEG_CLK 5
#define SERIAL_CLR 7

#define ONE_SECOND 1000

DispIO display_io(SERIAL_PIN, OUTSEG_CLK, SERIAL_CLK, SERIAL_CLR);
SegDisp display(display_io);

#define IR_LIGHT 9

HRMonitor monitor;

void setup()
{
    delay(ONE_SECOND);
    init_hr_listener();
    pinMode(IR_LIGHT, OUTPUT);
    digitalWrite(IR_LIGHT, HIGH);
    enable_hr_isr();
}

// extern void print_callbacks();

void loop()
{
    unsigned int hr;
    char ratebuf[4];
    delay(ONE_SECOND * 2);
    hr = monitor.get_rate();
    sprintf(ratebuf, "%04u", hr);
    display.write(ratebuf);
}
