/*
Author: Joshua Walker
Date: 18-June-2024
Version: 1.0
*/

#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "driver/ledc.h" //Chosen for simplicity
#include "driver/gpio.h"
#include "motor_ctrl.h"

/*
init_timer initializes the timer for the PWM module
args: motor a pointer to a struct representing a PWM DC Motor
*/
void init_timer(Motor_Struct* motor) {
	ledc_timer_config_t ledc_timer = {
		.speed_mode = motor->mode, 
		.duty_resolution = motor->duty_resolution, 
		.timer_num = motor->timer, 
		.freq_hz = motor->freq_hz, 
		.clk_cfg = motor->clk_cfg 
	};

	ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}

/*
init_channel initializes the channel for the PWM module
args: motor a pointer to a struct representing a PWM DC Motor
*/
void init_channel(Motor_Struct* motor) {
	ledc_channel_config_t ledc_channel = {
		.speed_mode = motor->mode, 
		.channel = motor->channel, 
		.timer_sel = motor->timer, 
		.intr_type = motor->intr_type, 
		.gpio_num = motor->pwm_pin, 
		.duty = motor->duty_resolution, 
		.hpoint = motor->hpoint //set phase shift. Typically 0
	};

	ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

/*
update_and_change_motor_speed sets the duty resolution (speed) for a channel then
updates the cchannel
args: motor a pointer to a struct representing a PWM DC Motor
*/
void update_and_change_motor_speed(Motor_Struct* motor) {
	ESP_ERROR_CHECK(ledc_set_duty(motor->mode, motor->channel, motor->duty_resolution)); 
	ESP_ERROR_CHECK(ledc_update_duty(motor->mode, motor->channel));
}

/*
init_motor_speed_buttons initializes the input GPIO pins for changing
motor speed 
args: motor a pointer to a struct representing a PWM DC Motor
*/
void init_motor_speed_buttons(Motor_Struct* motor) {
	//reset pin buffers
	ESP_ERROR_CHECK(gpio_reset_pin(motor->increase_pin));
	ESP_ERROR_CHECK(gpio_reset_pin(motor->decrease_pin));

	//Start Pins
	ESP_ERROR_CHECK(gpio_set_direction(motor->increase_pin, GPIO_MODE_INPUT));
	ESP_ERROR_CHECK(gpio_set_direction(motor->decrease_pin, GPIO_MODE_INPUT));
}

/*
motor_init initializes a dev board for use with a PWM DC Motor
args: motor a pointer to a struct representing a PWM DC Motor
*/
void motor_init(Motor_Struct* motor) {
	init_timer(motor);

	init_channel(motor);

	init_motor_speed_buttons(motor);
}

/*
motor_driver is the main driver for controlling a PWM motor.
args: args a void pointer passed in when the task is created.
*/
void motor_driver(void* args) {
	Motor_Struct* motor = (Motor_Struct*)args;
	update_and_change_motor_speed(motor);
	while(1) {
		if (!gpio_get_level(motor->decrease_pin) != !gpio_get_level(motor->increase_pin)) { //XOR Operation. Prevents acting on simultaneous button presses
			if (gpio_get_level(motor->decrease_pin) && motor->motor_power != ZERO_POWER) {
				motor->motor_power--;
				update_and_change_motor_speed(motor);
				while (gpio_get_level(motor->decrease_pin)) {
					delay(10);
				}
			}
			if (gpio_get_level(motor->increase_pin) && motor->motor_power != HIGH_POWER) {
				motor->motor_power++;
				update_and_change_motor_speed(motor);
				while (gpio_get_level(motor->increase_pin)) {
					delay(10);
				}
			}
		}
		delay(10);
	}
}
