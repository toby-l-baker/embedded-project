#ifndef BLE_RECEIVER_H
#define BLE_RECEIVER_H

#include "board_params.h"
#include "setup.h"
#include "simple_ble.h"

extern simple_ble_app_t* simple_ble_app;
extern bool manual_control;
extern bool power;
extern int8_t drive_speed;
extern int8_t turn_angle;
extern float* path_plan;

extern simple_ble_config_t ble_config;

extern simple_ble_service_t bike_srv;

extern simple_ble_char_t manual_char;

extern simple_ble_char_t power_cha;

extern simple_ble_char_t drive_char;

extern simple_ble_char_t turn_char;

extern simple_ble_char_t path_char;
void ble_evt_write(ble_evt_t const* p_ble_evt);
void init_ble_receiver(void);

// void ble_evt_write(ble_evt_t const* p_ble_evt);

#endif 