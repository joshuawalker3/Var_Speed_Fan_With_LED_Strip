#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "motor_ctrl.h"

void initialize_timer(Motor_Struct* motor) {

	ledc_timer_config_t ledc_timer = {
		.speed_mode = motor->mode, //set mode
		.duty_resolution = motor->duty_resolution, //set resolution
		.timer_num = motor->timer, //set timer
		.freq_hz = freq_hz, //set freq
		.clk_cfg = clk_cfg //set clock ticks
	};

	ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}

void initialize_channel(Motor_Struct* motor) {

	ledc_channel_config_t ledc_channel = {
		.speed_mode = speed_mode, //set speed
		.channel = channel, //set channel
		.timer_sel = timer_num, //set timer
		.intr_type = intr_type, //set interrupt type
		.gpio_num = gpio_num, //set pin
		.duty = duty, //set initial duty %
		.hpoint = hpoint //set phase shift
	};

	ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void update_and_change_motor_speed(
	ledc_mode_t speed_mode,
	ledc_channel_t channel,
	uint32_t duty) {

	ESP_ERROR_CHECK(ledc_set_duty(speed_mode, channel, duty)); //Update duty to zero for channel
	ESP_ERROR_CHECK(ledc_update_duty(speed_mode, channel)); //Applies changes to the channel
}

void initialize_motor_speed_buttons(int increase_speed_pin, int decrease_speed_pin) {
	//reset pin buffers
	ESP_ERROR_CHECK(gpio_reset_pin(increase_speed_pin));
	ESP_ERROR_CHECK(gpio_reset_pin(decrease_speed_pin));

	//Start Pins
	ESP_ERROR_CHECK(gpio_set_direction(increase_speed_pin, GPIO_MODE_INPUT));
	ESP_ERROR_CHECK(gpio_set_direction(decrease_speed_pin, GPIO_MODE_INPUT));

	increase_pin = increase_speed_pin;
	decrease_pin = decrease_speed_pin;

}

void default_motor_initialization(Motor_Struct* motor) {
	initialize_timer(LEDC_MODE, LEDC_DUTY_RES, LEDC_TIMER, LEDC_FREQUENCY, LEDC_CLK_CFG);

	initialize_channel(motor->pwm_pin, LEDC_MODE, LEDC_CHANNEL, LEDC_INTR_TYPE, LEDC_TIMER, ZERO_POWER, LEDC_PHASE_SHIFT);

	initialize_motor_speed_buttons(motor->increase_speed_pin, motor->decrease_speed_pin);

	xTaskCreate(motor_driver, "Motor Driver", 1024, NULL, 3, NULL);
}

void motor_driver(void* args) {
	while(1) {
		if (!gpio_get_level(decrease_pin) != !gpio_get_level(increase_pin)) { //XOR Operation
			if (gpio_get_level(decrease_pin) && motor_power != ZERO_POWER) {
				motor_power--;
				update_and_change_motor_speed(LEDC_MODE, LEDC_CHANNEL, motor_power);
				while (gpio_get_level(decrease_pin)) {
					delay(10);
				}
			}
			if (gpio_get_level(increase_pin) && motor_power != HIGH_POWER) {
				motor_power++;
				update_and_change_motor_speed(LEDC_MODE, LEDC_CHANNEL, motor_power);
				while (gpio_get_level(increase_pin)) {
					delay(10);
				}
			}
		}
	}
}
