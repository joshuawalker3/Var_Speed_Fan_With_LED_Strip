idf_component_register(SRCS "motor_ctrl_2_bit.c"
	INCLUDE_DIRS "include"
	PRIV_REQUIRES "esp_driver_ledc" "esp_driver_gpio")