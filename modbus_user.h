#ifndef _MODBUS_USER_
#define _MODBUS_USER_
#include "r_cg_macrodriver.h"
#include "modbus.h"

typedef enum temp_sensor_status
{
	TEMP_1_ALARM	= (1U << 0),
	TEMP_2_ALARM	= (1U << 1),
	TEMP_3_ALARM	= (1U << 2),
	TEMP_1_ERROR	= (1U << 3),
	TEMP_2_ERROR	= (1U << 4),
	TEMP_3_ERROR	= (1U << 5)
}MOD_TEMP_SEN_STATUS;

extern volatile uint8_t g_device_id;
extern MODBUS_SETTINGS g_modbus_settings;
extern volatile int16_t g_sp_offset_tx_buff[4];
extern volatile uint16_t g_temp_value_modbus[3];
extern volatile uint16_t g_alarm_err_status;

void init_and_start_modbus_slave_functions(void);
void (__near sp_offset_rx_call_back)(uint16_t start_reg, FUNCTION_CODE func_code, uint8_t no_of_data);

#endif