#include "modbus_user.h"
#include "ds18b20.h"
#include "flash.h"

// Variables
volatile uint8_t g_device_id; 
volatile int16_t g_sp_offset_tx_buff[4];
volatile int16_t g_sp_offset_rx_buff;
volatile uint16_t g_temp_value_modbus[3];
volatile uint16_t g_alarm_err_status, device_live_status;
MODBUS_SETTINGS g_modbus_settings;
MODBUS_MODULE g_modbus_modules[4];

void init_and_start_modbus_slave_functions(void)
{
	init_modbus(&g_modbus_settings,g_device_id,&P1,BIT5,&MK0H,BIT5,&MK0H,BIT6,&RXD0,&TXD0,&SSR00,&SMR00,5,4);
	create_modbus_module(&g_modbus_settings,&g_modbus_modules[0],READ_HOLDING_REGISTER,1000,4,(uint16_t *)g_sp_offset_tx_buff,
			     (uint16_t *)&g_sp_offset_rx_buff,sp_offset_rx_call_back);
	create_modbus_module(&g_modbus_settings,&g_modbus_modules[1],READ_INPUT_REGISTER,2000,3,(uint16_t *)g_temp_value_modbus,
			     NULL,NULL);
	create_modbus_module(&g_modbus_settings,&g_modbus_modules[2],READ_INPUT_STATUS,3000,6,(uint16_t *)&g_alarm_err_status,
			     NULL,NULL);
	create_modbus_module(&g_modbus_settings,&g_modbus_modules[3],READ_HOLDING_REGISTER,0,1,(uint16_t *)&device_live_status,
			     NULL,NULL);
	start_modbus_module(&g_modbus_settings,g_modbus_modules);
}

void (__near sp_offset_rx_call_back)(uint16_t reg_add, FUNCTION_CODE func_code, uint8_t no_of_data)
{
	g_sp_offset_tx_buff[reg_add - 1000]	= g_sp_offset_rx_buff;
	if((reg_add - 1000) == 0)
	{
		if(g_sp_offset_tx_buff[0] <= 350 && g_sp_offset_tx_buff[0] >= 150)
		{
			G_set_point				= g_sp_offset_tx_buff[0];
		}
	}
	else
	{
		if(g_sp_offset_tx_buff[reg_add - 1000] >= -50 && g_sp_offset_tx_buff[reg_add - 1000] <= 50)
		{
			G_ds18b20[0].offset			= g_sp_offset_tx_buff[1];
			G_ds18b20[1].offset			= g_sp_offset_tx_buff[2];
			G_ds18b20[2].offset			= g_sp_offset_tx_buff[3];
		}
	}
	write_set_point_offset_to_flash();
}
