#include "driver/ledc.h"

#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H

/*
stuff for pwm

#define LEDC_TIMER LEDC_TIMER_0 //1 of 4 timers
#define LEDC_MODE LEDC_LOW_SPEED_MODE //Changes to speed done by function call
#define LEDC_CHANNEL LEDC_CHANNEL_0 //1 of 8 channels
#define LEDC_DUTY_RES LEDC_TIMER_2_BIT //Duty resolution of 3 bits (2^2 - 1 discrete values)
#define LEDC_CLK_CFG LEDC_AUTO_CLK //Default clk ctrl
#define LEDC_INTR_TYPE LEDC_INTR_DISABLE //Disable interrupts
#define LEDC_FREQUENCY 4000 //4 kHz
#define LEDC_PHASE_SHIFT 0 //0 phase shift
*/

/*
Power Levels
*/
typedef enum {
	ZERO_POWER,
	LOW_POWER,
	HIGH_POWER
}power_level;

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

/*
standard delay
*/
#ifndef delay
#define delay(time) vTaskDelay(time / portTICK_PERIOD_MS) //delays for time ms
#endif

void initialize_timer(Motor_Struct* motor);

void initialize_channel(Motor_Struct* motor);

void update_and_change_motor_speed(Motor_Struct motor);

void initialize_motor_speed_buttons(int increase_speed_pin, int decrease_speed_pin);

void motor_initialization(Motor_Struct* motor);

void motor_driver(void* args);

#endif