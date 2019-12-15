#include "ble_receiver.h"


simple_ble_app_t* simple_ble_app = NULL;
float* path_plan = NULL;

// Intervals for advertising and connections
simple_ble_config_t ble_config = {
        // c0:98:e5:49:xx:xx
        .platform_id       = 0x49,    // used as 4th octect in device BLE address
        .device_id         = 0x0069, // TODO: replace with your lab bench number
        .adv_name          = "KOBUKI", // used in advertisements if there is room
        .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS),
};
simple_ble_service_t bike_srv = {{
    .uuid128 = {0xca,0xa4,0x87,0x0d,0x42,0x84,0xff,0xA9,
                0x59,0x4D,0x5e,0xf6,0xa0,0xed,0x07,0x46}
}};

simple_ble_char_t manual_char = {.uuid16 = 0xeda1};
bool manual_control=0;

simple_ble_char_t power_char = {.uuid16 = 0xeda2};
bool power=0;

simple_ble_char_t drive_char = {.uuid16 = 0xeda3};
int8_t drive_speed=0;

simple_ble_char_t turn_char = {.uuid16 = 0xeda4};
int8_t turn_angle=0;

simple_ble_char_t path_char = {.uuid16 = 0xeda5};
float* path_plan;

void ble_evt_write(ble_evt_t const* p_ble_evt) {
    // TODO: logic for each characteristic and related state changes

    if (simple_ble_is_char_event(p_ble_evt, &manual_char)) {
      if (manual_control) {
        bike_state = MANUAL;
      } else {
        bike_state = AUTONOMOUS;
      }

    } else if (simple_ble_is_char_event(p_ble_evt, &power_char)) {
      if (power) {
        bike_state = MANUAL;
      } else {
        bike_state = OFF;
      }
    } 
}



void init_ble_receiver(void){
	simple_ble_app = simple_ble_init(&ble_config);
	simple_ble_add_service(&bike_srv);

	// Start Advertising
	simple_ble_adv_only_name();

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(manual_control), (uint8_t*)&manual_control,
	  &bike_srv, &manual_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(power), (uint8_t*)&power,
	  &bike_srv, &power_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(drive_speed), (uint8_t*)&drive_speed,
	  &bike_srv, &drive_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(turn_angle), (uint8_t*)&turn_angle,
	  &bike_srv, &turn_char);

	simple_ble_add_characteristic(1, 1, 0, 0,
	  sizeof(*path_plan), (float*)path_plan,
	  &bike_srv, &path_char);
	printf("BLE Receiver Initialized.\n");


}