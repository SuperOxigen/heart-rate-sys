#include "hr_monitor.hpp"
#include <Arduino.h>
#include <stdio.h>

// Do not want to many, otherwise ISR will take too long.
#define MAX_HR_CALLBACKS 3

static HRMonitor *hr_callbacks[MAX_HR_CALLBACKS];
static void hr_isr(void);
static bool hr_isr_enabled = false;

void init_hr_listener(pin_t listener_pin)
{
    attachInterrupt(digitalPinToInterrupt(listener_pin), hr_isr, RISING);
}

void enable_hr_isr()
{
    hr_isr_enabled = true;
}

void disable_hr_isr()
{
    hr_isr_enabled = false;
}

void print_callbacks()
{
    char buf[25];
    for (int i = 0; i < MAX_HR_CALLBACKS; i++)
    {
        sprintf(buf, "%d - 0x%X", i, hr_callbacks[i]);
        Serial.println(buf);
    }
}

static void hr_isr()
{
    if (!hr_isr_enabled) return;
    for (int i = 0; i < MAX_HR_CALLBACKS; i++)
    {
        if (hr_callbacks[i] != NULL)
        {
            hr_callbacks[i]->isr();
        }
    }
}

static bool subscribe_monitor(HRMonitor * hrm)
{
    for (int i = 0; i < MAX_HR_CALLBACKS; i++)
    {
        if (hr_callbacks[i] == NULL)
        {
            char buf[25];
            // sprintf(buf, "Subscribed: 0x%X", hrm);
            // Serial.println(buf);
            hr_callbacks[i] = hrm;
            return true;
        }
    }
    return false;
}

static void unsubscribe_monitor(HRMonitor * hrm)
{
    for (int i = 0; i < MAX_HR_CALLBACKS; i++)
    {
        if (hr_callbacks[i] == hrm)
        {
            // Serial.println("Unsubscribing");
            hr_callbacks[i] = NULL;
            return;
        }
    }
}

static inline unsigned int inc_ptr(unsigned int ptr)
{
    return (ptr + 1) % HR_COUNTERS;
}

HRMonitor::HRMonitor()
{
    mtime_t now = millis();
    last_valid = 0;

    for (int i = 0; i < HR_COUNTERS; i++)
    {
        last_pull[i] = now;
        beat_counts[i] = 0;
    }

    pull_ptr = 0;
    // Serial.println("Subscribing!");
    subscribed = subscribe_monitor(this);
}

HRMonitor::~HRMonitor()
{
    if (subscribed)
        unsubscribe_monitor(this);
}

void HRMonitor::isr()
{
    // IMPORTANT: Keep this short as possible.
    mtime_t now = millis();
    // Serial.println("Pulse!");
    if ((now - last_valid) > 500ul)
    {
        // Serial.println("Up count");
        beat_counts[pull_ptr]++;
        last_valid = now;
    }
}

unsigned int HRMonitor::total_beats()
{
    unsigned int total = 0;
    for (int i = 0; i < HR_COUNTERS; i++)
    {
        total += beat_counts[i];
    }
    return total;
}

unsigned int HRMonitor::get_rate()
{
    mtime_t start, end, delta;
    unsigned int total;

    pull_ptr = inc_ptr(pull_ptr);
    end = last_valid;
    start = last_pull[pull_ptr];
    total = total_beats();
    last_pull[pull_ptr] = millis();
    beat_counts[pull_ptr] = 0;

    delta = (end - start);
    if (delta == 0) return 0;
    else return (total * 60000ul) / delta;
}
