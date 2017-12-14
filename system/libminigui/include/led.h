#ifndef LED_H
#define LED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LED_MODULE_ID "led"

int indicator_led_switch(int sw);

void set_indicator_led_flicker(int times);

void start_indicator_led_flicker_thread(int period_ms);
void stop_indicator_led_flicker_thread(void);

int fill_light_switch(int sw);

#ifdef __cplusplus
}
#endif

#endif

