#include "tail_lights.h"

nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);
#define RIGHT_BLINKER_STRING	"              >>"
#define LEFT_BLINKER_STRING		"<<              "
#define BRAKE_STRING			"|||          |||"
#define BOTH_BLINKER_STRING		"<<            >>"
#define EMPTY_STRING			"                "

bool Right_Blinker = false;
bool Left_Blinker = false;

void init_tail_lights(){
	
	nrf_drv_spi_config_t spi_config = {
		.sck_pin = BUCKLER_LCD_SCLK,
		.mosi_pin = BUCKLER_LCD_MOSI,
		.miso_pin = BUCKLER_LCD_MISO,
		.ss_pin = BUCKLER_LCD_CS,
		.irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
		.orc = 0,
		.frequency = NRF_DRV_SPI_FREQ_4M,
		.mode = NRF_DRV_SPI_MODE_2,
		.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
	};
	ret_code_t error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
	APP_ERROR_CHECK(error_code);
	display_init(&spi_instance);

	return;
}


void update_lights(angles_t * angles){



}

void blinker_toggle(){
	static bool light = false;

	if (light){
		display_write(BOTH_BLINKER_STRING, DISPLAY_LINE_1);
		
	}
	else{
		display_write(RIGHT_BLINKER_STRING, DISPLAY_LINE_1);
	}


	light = !light;
}