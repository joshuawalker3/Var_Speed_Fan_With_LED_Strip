/*
Author: Joshua Walker
Date: 18-June-2024
Version: 1.0
*/

#ifndef LED_CTRL_H
#define LED_CTRL_H

#include "freertos/task.h"

/*
Delay Macro
*/
#ifndef delay
#define delay(time) vTaskDelay(time / portTICK_PERIOD_MS) //delays for time ms
#endif

/*
Pin CTRL
*/
#define ON 1
#define OFF 0

/*
Color Combinations
*/
typedef enum {
	ALL_OFF,
	RED,
	GREEN,
	BLUE,
	RED_GREEN,
	RED_BLUE,
	GREEN_BLUE,
	ALL_ON
}Combination;

/*
Modes
*/
typedef enum {
	STATIC = 1,
	CYCLE = -1
}Mode;

/*
Struct representing a RGB LED Strip
Static_Mode_Handle and Cycle_Mode_Handle are typically initialized as
NULL as the driver will update their values
*/
typedef struct {
	int red_pin;
	int green_pin;
	int blue_pin;
	int cycle_color_button;
	int cycle_mode_button;
	Combination light_combo;
	Mode curr_mode;
	TaskHandle_t Static_Mode_Handle;
	TaskHandle_t Cycle_Mode_Handle;
}LED_Struct;

void led_strip_led_init(LED_Struct* LED_Strip);

void color_button_init(LED_Struct* LED_Strip);

void mode_button_init(LED_Struct* LED_Strip);

void led_strip_init(LED_Struct* LED_Strip);

void set_led_color_switch(LED_Struct* LED_Strip);

void set_led_color(LED_Struct* LED_Strip, int red, int green, int blue);

void led_strip_static_color(void* args);

void led_strip_cycle_colors(void* args);

void led_driver(void* args);

#endif