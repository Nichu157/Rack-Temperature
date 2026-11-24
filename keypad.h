#include "r_cg_macrodriver.h"
#include "variable.h"

#ifndef _KEYPAD_

#define _KEYPAD_

#define K1			P0_bit.no0
#define K2			P13_bit.no7
#define K3			P3_bit.no1
#define K4			P6_bit.no1

#define ENTER_EXIT		0x06
#define ENTER			0x0E		// 0B 0000 1111
#define UP			0x0D
#define DOWN			0x0B
#define EXIT			0x07

#define K_100_MS		10
#define K_250_MS		25


typedef enum
{
	NONE,
	RUN_MODE,
	CONFIG_MODE,
	SENSOR_1_CONFIG,
	SENSOR_2_CONFIG,
	SENSOR_3_CONFIG,
	READ_SENSOR_1_ROM,
	READ_SENSOR_2_ROM,
	READ_SENSOR_3_ROM,
	DEVICE_ID,
	DEVICE_ID_EDIT,
	DEVICE_ID_UPD,
	SET_POINT,
	SET_POINT_EDIT,
	SET_POINT_UPD,
	OFFSET_1,
	OFFSET_2,
	OFFSET_3,
	OFFSET_1_EDIT,
	OFFSET_2_EDIT,
	OFFSET_3_EDIT,
	OFFSET_1_UPD,
	OFFSET_2_UPD,
	OFFSET_3_UPD
}MENU_STATUS;
// Global variales
extern BYTE_TO_BITFIELD key_status;
extern BITFIELD uc;

extern volatile MENU_STATUS G_menu_state_machine;
extern volatile uint8_t G_key_scan_timer;

//Function declaration
void process_enter_key_routine(void);
void process_up_key_routine(void);
void process_down_key_routine(void);
void process_exit_key_routine(void);
void handle_display_after_updation(void);
void key_service_routine(void);

#endif