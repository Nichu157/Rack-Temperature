#include "r_cg_macrodriver.h"
#include "variable.h"
#include "keypad.h"
#include "ds18b20.h"
// All extern-variables defined here

// extern-structure and union
BYTE_TO_BITFIELD key_status,seven_seg_led;
BITFIELD uc;
SENSOR_DATA G_ds18b20[3];

// Extern-variable
volatile uint8_t G_display_data[4];
volatile uint8_t G_display_delay_tmr;
volatile MENU_STATUS G_menu_state_machine;
volatile uint8_t G_key_scan_timer;

volatile uint8_t G_usec5;
volatile uint8_t G_rom_search_timer;
volatile uint8_t G_rom_code[3][10];
volatile uint16_t G_temperature;

volatile uint8_t G_temp_disp_tmr;

volatile DS_STATE G_ds18b20_state;
volatile uint8_t G_disp_state_on_run_mode;

volatile uint16_t G_set_point;