/*
Author: Joshua Walker
Date: 19-June-2024
Version: 1.0

Var_Speed_Fan_With_LED_Strip.c initializes an ESP32 DEVKIT Board
for use with 1 12V Brushless DC Fan (reffered to as motor) and 
1 12V RGB LED Strip (referred to as LED_Strip).
Upon completion of initialization, the drivers for both components
are started and app_main returns.

The final .elf was built using ESP-IDF v5.3-dev with a set target of ESP32.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_ctrl.h"
#include "motor_ctrl_2_bit.h"

/*
Constants for motor. 
*/
#define MOTOR_TIMER LEDC_TIMER_0 //1 of 4 timers
#define MOTOR_MODE LEDC_LOW_SPEED_MODE //Changes to speed done by function call
#define MOTOR_CHANNEL LEDC_CHANNEL_0 //1 of 8 channels
#define MOTOR_CLK_CFG LEDC_AUTO_CLK //Default clk ctrl
#define MOTOR_INTR_TYPE LEDC_INTR_DISABLE //Disable interrupts
#define MOTOR_FREQUENCY 4000 //4 kHz
#define MOTOR_PHASE_SHIFT 0 //0 phase shift
#define MOTOR_PWM_PIN 19 //Motor PWM Pin
#define MOTOR_DECREASE_PIN 32 //Decrease speed button
#define MOTOR_INCREASE_PIN 33 //Increase speed button

/*
Constants for LED Strip
*/
#define LED_RED_PIN 2
#define LED_GREEN_PIN 4
#define LED_BLUE_PIN 5
#define LED_CYCLE_COLOR_PIN 34
#define LED_CYCLE_MODE_PIN 35

#define STACK_STD 2048 //std stack size for drivers

Motor_Struct_2_Bit motor = {
	.timer = MOTOR_TIMER,
	.mode = MOTOR_MODE,
	.channel = MOTOR_CHANNEL,
	.clk_cfg = MOTOR_CLK_CFG,
	.intr_type = MOTOR_INTR_TYPE,
	.freq_hz = MOTOR_FREQUENCY,
	.hpoint = MOTOR_PHASE_SHIFT,
	.pwm_pin = MOTOR_PWM_PIN,
	.decrease_pin = MOTOR_DECREASE_PIN,
	.increase_pin = MOTOR_INCREASE_PIN,
	.motor_power = ZERO_POWER
};

LED_Struct LED_Strip = {
	.red_pin = LED_RED_PIN,
	.green_pin = LED_GREEN_PIN,
	.blue_pin = LED_BLUE_PIN,
	.cycle_color_button = LED_CYCLE_COLOR_PIN,
	.cycle_mode_button = LED_CYCLE_MODE_PIN,
	.light_combo = RED,
	.curr_mode = STATIC
};

void app_main(void) {
	led_strip_init(&LED_Strip);

	motor_init(&motor);

	xTaskCreate(motor_driver, "Motor Driver", STACK_STD, (void*)&motor, 2, NULL);
	
	xTaskCreate(led_driver, "LED Strip Driver", STACK_STD, (void*)&LED_Strip, 2, NULL);
}
