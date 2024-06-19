#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "led_ctrl.h"

void led_init(int red, int green, int blue) {
	//reset pin buffers
	ESP_ERROR_CHECK(gpio_reset_pin(red));
	ESP_ERROR_CHECK(gpio_reset_pin(green));
	ESP_ERROR_CHECK(gpio_reset_pin(blue));

	//Start Pins
	ESP_ERROR_CHECK(gpio_set_direction(red, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_direction(green, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_direction(blue, GPIO_MODE_OUTPUT));
}

void cycle_button_init(int button) {
	ESP_ERROR_CHECK(gpio_reset_pin(button));
	ESP_ERROR_CHECK(gpio_set_direction(button, GPIO_MODE_INPUT));
}

void mode_button_init(int button) {
	ESP_ERROR_CHECK(gpio_reset_pin(button));
	ESP_ERROR_CHECK(gpio_set_direction(button, GPIO_MODE_INPUT));
}

void default_led_strip_init(LED_Struct* LED_Strip) {
	led_init(LED_Strip->red_pin, LED_Strip->green_pin, LED_Strip->blue_pin);
	cycle_button_init(LED_Strip->cycle_color_button);
	mode_button_init(LED_Strip->cycle_mode_button);
	LED_Strip->light_combo = RED;
	LED_Strip->curr_mode = STATIC;
	xTaskCreate(led_strip_static_color, "LED Static Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Static_Mode_Handle));
}

void led_strip_static_color(void* args) {
	LED_Struct* LED_Strip = (LED_Struct*)args;
	while (1) {
		if (gpio_get_level(LED_Strip->cycle_color_button)) {
			if (LED_Strip->light_combo == ALL_ON) {
				LED_Strip->light_combo = ALL_OFF;
			}
			else {
				LED_Strip->light_combo++;
			}

			set_led_color_switch(LED_Strip);

			while (gpio_get_level(LED_Strip->cycle_color_button)) {
				delay(10);
			}
		}
		delay(10);
	}
}

void led_strip_cycle_colors(void* args) {
	LED_Struct* LED_Strip = (LED_Struct*)args;
	while (1) {
		set_led_color_switch(LED_Strip);
		if (LED_Strip->light_combo == ALL_ON) {
			LED_Strip->light_combo = RED;
		}
		else {
			LED_Strip->light_combo++;
		}
		
		delay(1000);
	}
}

void led_mode_handler(void* args) {
	LED_Struct* LED_Strip = (LED_Struct*)args;
	while (1) {
		if (gpio_get_level(LED_Strip->cycle_mode_button)) {
			LED_Strip->curr_mode = -(LED_Strip->curr_mode);

			if (LED_Strip->curr_mode == STATIC) {
				vTaskDelete(LED_Strip->Cycle_Mode_Handle);
				LED_Strip->Cycle_Mode_Handle = NULL;
				xTaskCreate(led_strip_static_color, "LED Static Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Static_Mode_Handle));
			}
			else {
				vTaskDelete(LED_Strip->Static_Mode_Handle);
				LED_Strip->Static_Mode_Handle = NULL;

				if (LED_Strip->light_combo == ALL_OFF) {
					LED_Strip->light_combo = RED;
				}

				xTaskCreate(led_strip_cycle_colors, "LED Cycle Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Static_Mode_Handle));
			}

			while (gpio_get_level(LED_Strip->cycle_mode_button)) {
				delay(10);
			}
		}
		delay(10);
	}
}

void set_led_color_switch(LED_Struct* LED_Strip) {
	switch (LED_Strip->light_combo) {
		case ALL_OFF:
			set_led_color(LED_Strip, OFF, OFF, OFF);
			break;
		case RED:
			set_led_color(LED_Strip, ON, OFF, OFF);
			break;
		case GREEN:
			set_led_color(LED_Strip, OFF, ON, OFF);
			break;
		case BLUE:
			set_led_color(LED_Strip, OFF, OFF, ON);
			break;
		case RED_GREEN:
			set_led_color(LED_Strip, ON, ON, OFF);
			break;
		case RED_BLUE:
			set_led_color(LED_Strip, ON, OFF, ON);
			break;
		case GREEN_BLUE:
			set_led_color(LED_Strip, OFF, ON, ON);
			break;
		case ALL_ON:
			set_led_color(LED_Strip, ON, ON, ON);
			break;
		default:
			break;
	}
}

void set_led_color(LED_Struct* LED_Strip, int red, int green, int blue) {
	gpio_set_level(LED_Strip->red_pin, red);
	gpio_set_level(LED_Strip->green_pin, green);
	gpio_set_level(LED_Strip->blue_pin, blue);
}

