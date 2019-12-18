#ifndef BLE_RECEIVER_H
#define BLE_RECEIVER_H

#include "board_params.h"
#include "setup.h"
#include "simple_ble.h"

extern simple_ble_app_t* simple_ble_app;
extern bool manual_control; // used to move the bike between manual and autonomous modes
extern bool power; // used to move the bike from manual to off
extern int8_t drive_speed; // drive speed in manual mode
extern int8_t turn_angle; // turn angle in manual moe
extern uint8_t path_len; // path len for autonomous mode
extern int8_t path_angle; // path angle for autonomous mode
extern bike_state bike_state;
extern bool path_len_received = false;
extern bool path_angle_received = false;

extern simple_ble_config_t ble_config;

extern simple_ble_service_t bike_srv;

extern simple_ble_char_t manual_char;

extern simple_ble_char_t power_char;

extern simple_ble_char_t drive_char;

extern simple_ble_char_t turn_char;

extern simple_ble_char_t path_len_char;

extern simple_ble_char_t path_angle_char;

void ble_evt_write(ble_evt_t const* p_ble_evt);
void init_ble_receiver(void);

#endif
