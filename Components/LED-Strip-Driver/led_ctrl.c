/*
Author: Joshua Walker
Date: 18-June-2024
Version: 1.0
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "led_ctrl.h"


/**
led_strip_led_init initializes the output GPIO pinsfor an RGB LED Strip
args: LED_Strip a pointer to a struct representing a RGB LED Strip
*/
void led_strip_led_init(LED_Struct* LED_Strip) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "Initializing LED Pins...");

	//reset pin buffers
	ESP_ERROR_CHECK(gpio_reset_pin(LED_Strip->red_pin));
	ESP_ERROR_CHECK(gpio_reset_pin(LED_Strip->green_pin));
	ESP_ERROR_CHECK(gpio_reset_pin(LED_Strip->blue_pin));

	//Start Pins
	ESP_ERROR_CHECK(gpio_set_direction(LED_Strip->red_pin, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_direction(LED_Strip->green_pin, GPIO_MODE_OUTPUT));
	ESP_ERROR_CHECK(gpio_set_direction(LED_Strip->blue_pin, GPIO_MODE_OUTPUT));
}

/*
color_button_init initizlizes the input for the input GPIO pin connected
to a tactile switch to allow for switching colors in STATIC mode
args: LED_Strip a pointer to a struct representing a RGB LED Strip
*/
void color_button_init(LED_Struct* LED_Strip) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "Initializing LED Color Change Button...");

	//reset pin buffers
	ESP_ERROR_CHECK(gpio_reset_pin(LED_Strip->cycle_color_button));

	//Start Pins
	ESP_ERROR_CHECK(gpio_set_direction(LED_Strip->cycle_color_button, GPIO_MODE_INPUT));
}

/*
mode_button_init initizlizes the input for the input GPIO pin connected
to a tactile switch to allow for switching of modes
args: LED_Strip a pointer to a struct representing a RGB LED Strip
*/
void mode_button_init(LED_Struct* LED_Strip) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "Initializing LED Mode Change Button...");

	//reset pin buffers
	ESP_ERROR_CHECK(gpio_reset_pin(LED_Strip->cycle_mode_button));

	//Start Pins
	ESP_ERROR_CHECK(gpio_set_direction(LED_Strip->cycle_mode_button, GPIO_MODE_INPUT));
}

/*
led_strip_init initializes a the dev board for use with an RGB LED strip
args: LED_Strip a pointer to a struct representing a RGB LED Strip
*/
void led_strip_init(LED_Struct* LED_Strip) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "Initializing LED Strip...");

	led_strip_led_init(LED_Strip);

	color_button_init(LED_Strip);

	mode_button_init(LED_Strip);

	ESP_LOGI(taskName, "Completed Initializing LED Strip...");
}

/*
led_strip_static_color is the driver for when the mode is STATIC
args: args a void pointer passed in when the task is created
*/
void led_strip_static_color(void* args) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "LED STATIC Color Task Started!");

	LED_Struct* LED_Strip = (LED_Struct*)args;

	set_led_color_switch(LED_Strip);

	while (1) {
		if (gpio_get_level(LED_Strip->cycle_color_button)) {
			int counter = 0; //Counter to determine mhow long button has been pressed in increments of hundredths of a second 

			while (gpio_get_level(LED_Strip->cycle_color_button)) {
				counter++;
				delay(10);
			}

			if (counter >= 100) {
				LED_Strip->light_combo = ALL_OFF;
			}
			else if (LED_Strip->light_combo == ALL_ON) {
				LED_Strip->light_combo = RED;
			}
			else {
				LED_Strip->light_combo++;
			}

			set_led_color_switch(LED_Strip);
		}
		delay(10);
	}
}

/*
led_strip_cycle_color is the driver for when the mode is CYCLE
args: args a void pointer passed in when the task is created
*/
void led_strip_cycle_colors(void* args) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "LED CYCLE Color Task Started!");

	LED_Struct* LED_Strip = (LED_Struct*)args;

	if (LED_Strip->light_combo == ALL_OFF) {
		LED_Strip->light_combo = ALL_ON - 1; 
	}

	while (1) {
		if (LED_Strip->light_combo == ALL_ON) {
			LED_Strip->light_combo = RED;
		}
		else {
			LED_Strip->light_combo++;
		}

		set_led_color_switch(LED_Strip);
		
		delay(1000);
	}
}

/*
led_driver is the main driver for the LED strip. It switches tasks whenever the mode change
signal is received.
args: args a void pointer passed in when the task is created. Typically NULL
*/
void led_driver(void* args) {
	char* taskName = pcTaskGetName(NULL);

	ESP_LOGI(taskName, "LED Driver started....");

	LED_Struct* LED_Strip = (LED_Struct*)args;
	
	if (LED_Strip->curr_mode == STATIC) {
		xTaskCreate(led_strip_static_color, "LED Static Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Static_Mode_Handle));

		ESP_LOGI(taskName, "Initial LED Mode STATIC!");
	}
	else {
		xTaskCreate(led_strip_cycle_colors, "LED Cycle Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Cycle_Mode_Handle));

		ESP_LOGI(taskName, "Initial LED Mode CYCLE!");
	}
	
	while (1) {
		if (gpio_get_level(LED_Strip->cycle_mode_button)) {
			LED_Strip->curr_mode = -(LED_Strip->curr_mode);

			if (LED_Strip->curr_mode == STATIC) {
				vTaskDelete(LED_Strip->Cycle_Mode_Handle);
				LED_Strip->Cycle_Mode_Handle = NULL;

				xTaskCreate(led_strip_static_color, "LED Static Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Static_Mode_Handle));

				ESP_LOGI(taskName, "LED Mode changed to STATIC!");
			}
			else {
				vTaskDelete(LED_Strip->Static_Mode_Handle);
				LED_Strip->Static_Mode_Handle = NULL;

				xTaskCreate(led_strip_cycle_colors, "LED Cycle Mode", 2048, (void*)LED_Strip, 2, &(LED_Strip->Cycle_Mode_Handle));

				ESP_LOGI(taskName, "LED Mode changed to CYCLE!");
			}

			while (gpio_get_level(LED_Strip->cycle_mode_button)) {
				delay(10);
			}
		}
		delay(10);
	}
}

/*
set_led_color_switch is a switch for determining what LEDs in the LED strip
need to be illuminated based upon current light combo.
args: LED_Strip a pointer to a struct representing a RGB LED Strip
*/
void set_led_color_switch(LED_Struct* LED_Strip) {
	char* taskName = pcTaskGetName(NULL);

	switch (LED_Strip->light_combo) {
		case ALL_OFF:
			set_led_color(LED_Strip, OFF, OFF, OFF);

			ESP_LOGI(taskName, "All LEDs Off!");

			break;
		case RED:
			set_led_color(LED_Strip, ON, OFF, OFF);

			ESP_LOGI(taskName, "Red LED On!");

			break;
		case GREEN:
			set_led_color(LED_Strip, OFF, ON, OFF);

			ESP_LOGI(taskName, "Green LED On!");

			break;
		case BLUE:
			set_led_color(LED_Strip, OFF, OFF, ON);

			ESP_LOGI(taskName, "Blue LED On!");

			break;
		case RED_GREEN:
			set_led_color(LED_Strip, ON, ON, OFF);

			ESP_LOGI(taskName, "Red And Green LEDs On!");

			break;
		case RED_BLUE:
			set_led_color(LED_Strip, ON, OFF, ON);

			ESP_LOGI(taskName, "Red And Blue LEDs On!");

			break;
		case GREEN_BLUE:
			set_led_color(LED_Strip, OFF, ON, ON);

			ESP_LOGI(taskName, "Green And Blue LEDs On!");

			break;
		case ALL_ON:
			set_led_color(LED_Strip, ON, ON, ON);

			ESP_LOGI(taskName, "All LEDs On! It sure is bright!");
			
			break;
		default:
			break;
	}
}

/*
set_led_color sets GPIO output pin levels based upon request sent by set_led_color_switch
args: LED_Strip a pointer to a struct representing a RGB LED Strip
args: red, the requested state of red LED
args: green, the requested state of green LED
args: blue, the requested state of blue LED
*/
void set_led_color(LED_Struct* LED_Strip, int red, int green, int blue) {
	gpio_set_level(LED_Strip->red_pin, red);
	gpio_set_level(LED_Strip->green_pin, green);
	gpio_set_level(LED_Strip->blue_pin, blue);
}

