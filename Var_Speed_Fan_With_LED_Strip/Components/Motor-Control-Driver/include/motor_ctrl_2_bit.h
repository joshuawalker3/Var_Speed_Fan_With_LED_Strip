/*
Author: Joshua Walker
Date: 18-June-2024
Version: 1.0
*/

#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H

#include "driver/ledc.h"

/*
standard delay
*/
#ifndef delay
#define delay(time) vTaskDelay(time / portTICK_PERIOD_MS) //delays for time ms
#endif

/*
Power Levels for 2 bit resolution
*/
typedef enum {
	ZERO_POWER = 0,
	LOW_POWER = 2,
	HIGH_POWER = 4
}Power_Level;

/*
Struct representing a PWM DC Motor. Duty resolution is 2-bits
*/
typedef struct {
	ledc_timer_t timer;
	ledc_mode_t mode;
	ledc_channel_t channel;
	ledc_clk_cfg_t clk_cfg;
	ledc_intr_type_t intr_type;
	uint32_t freq_hz;
	int hpoint;
	int pwm_pin;
	int decrease_pin;
	int increase_pin;
	Power_Level motor_power;
}Motor_Struct_2_Bit;

void init_timer(Motor_Struct_2_Bit* motor);

void init_channel(Motor_Struct_2_Bit* motor);

void update_and_change_motor_speed(Motor_Struct_2_Bit* motor);

void init_motor_speed_buttons(Motor_Struct_2_Bit* motor);

void motor_init(Motor_Struct_2_Bit* motor);

void motor_driver(void* args);

#endif