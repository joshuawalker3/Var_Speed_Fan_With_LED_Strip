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
Power Levels
*/
typedef enum {
	ZERO_POWER,
	LOW_POWER,
	HIGH_POWER
}power_level;

/*
Struct representing a PWM DC Motor
*/
typedef struct {
	ledc_timer_t timer;
	ledc_mode_t mode;
	ledc_channel_t channel;
	uint32_t duty_resolution;
	ledc_clk_cfg_t clk_cfg;
	ledc_intr_type_t intr_type;
	uint32_t freq_hz;
	int hpoint;
	int pwm_pin;
	int decrease_pin;
	int increase_pin;
	power_level motor_power;
}Motor_Struct;

void init_timer(Motor_Struct* motor);

void init_channel(Motor_Struct* motor);

void update_and_change_motor_speed(Motor_Struct* motor);

void init_motor_speed_buttons(Motor_Struct* motor);

void motor_init(Motor_Struct* motor);

void motor_driver(void* args);

#endif