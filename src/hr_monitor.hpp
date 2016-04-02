#pragma once

#include "hr_utils.hpp"

#define HR_COUNTERS 10

class HRMonitor
{
    bool subscribed;
    mtime_t last_valid;
    mtime_t last_pull[HR_COUNTERS];
    unsigned int pull_ptr;
    unsigned int beat_counts[HR_COUNTERS];

public:
    HRMonitor();
    HRMonitor(HRMonitor const &) = delete;
    HRMonitor(HRMonitor &&) = delete;
    ~HRMonitor();

    HRMonitor & operator=(HRMonitor const &) = delete;
    HRMonitor & operator=(HRMonitor &&) = delete;

    void isr();

    unsigned int total_beats();
    unsigned int get_rate();
    void reset();
    explicit operator bool () const { return subscribed; }
};

void init_hr_listener(pin_t listener_pin = DEFAULT_INTERRUPT_PIN);
void enable_hr_isr();
void disable_hr_isr();
