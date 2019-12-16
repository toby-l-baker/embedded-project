#include "buzzer.h"

void init_buzzer(void){
	// This function initializes a buzzer using an nrf gpio module
	nrf_gpio_cfg_output(BUZZER_PIN);
	nrf_gpio_pin_clear(BUZZER_PIN);
	return;
}

void buzzer_on(void){
	// This function turns on a buzzer
	nrf_gpio_pin_set(BUZZER_PIN);
	return;
}

void buzzer_off(void){
	// This function turns off a buzzer
	nrf_gpio_pin_clear(BUZZER_PIN);
	return;
}