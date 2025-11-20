#ifndef TIMER_H
#define TIMER_H

// PIT ports
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42
#define PIT_COMMAND     0x43

// PIT command bits
#define PIT_CHANNEL0_SELECT    0x00
#define PIT_ACCESS_LOBYTE      0x10
#define PIT_ACCESS_HIBYTE      0x20
#define PIT_MODE_RATE_GEN      0x04
#define PIT_MODE_SQUARE_WAVE   0x06

// Timer frequency (Hz)
#define TIMER_FREQUENCY 100  // 100 Hz = 10ms per tick

// Initialize PIT timer
void timer_init(void);

// Get current tick count
unsigned long long timer_get_ticks(void);

// Sleep for specified milliseconds
void timer_sleep(unsigned long long milliseconds);

#endif // TIMER_H

