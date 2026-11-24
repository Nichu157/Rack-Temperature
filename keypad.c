#include "keypad.h"
#include "display.h"
#include "ds18b20.h"
#include "flash.h"
#include "modbus_user.h"
#include "user_io_define.h"

// Static variables
int16_t value_to_edit;
uint8_t data_to_display[4];

// Static function declaration
static void check_sign_and_display_offset_value(void);

// Handle key process
void key_service_routine(void)
{
	switch(key_status.byte)
	{
		case ENTER:
			process_enter_key_routine();
		break;
		case UP:
			process_up_key_routine();
		break;
		case DOWN:
			process_down_key_routine();
		break;
		case EXIT:
			process_exit_key_routine();
		break;
		default:
		break;
	}
}

// Handle enter key stuffs
void process_enter_key_routine(void)
{
	switch(G_menu_state_machine)
	{
		case CONFIG_MODE:
			G_menu_state_machine	=	SENSOR_1_CONFIG;
	    		change_display_value_to(" t1",DOT_OFF);
			T1_LED_ON;
		break;
		case SENSOR_1_CONFIG:
			G_menu_state_machine	=	READ_SENSOR_1_ROM;
			change_display_value_to("pro",DOT_OFF);
			
			G_temp_disp_tmr		= 	TAU2_1_SEC;
			uc.temp_disp_delay	=	1;
			while(!uc.call_ds18b20_fun);
			uc.call_ds18b20_fun	=	0;	// 1 second delay
			
			if(ROM_CODE == read_rom_code_from_ds18b20(&G_rom_code[0][0]))
			{
				change_display_value_to("Upd",DOT_OFF);
			}
			else if(ROM_CODE != read_rom_code_from_ds18b20(&G_rom_code[0][0]))
			{
				change_display_value_to("Err",DOT_OFF);
			}
			write_rom_code_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case SENSOR_2_CONFIG:
			G_menu_state_machine	=	READ_SENSOR_2_ROM;
			change_display_value_to("pro",DOT_OFF);
			
			G_temp_disp_tmr		= 	TAU2_1_SEC;
			uc.temp_disp_delay	=	1;
			while(!uc.call_ds18b20_fun);
			uc.call_ds18b20_fun	=	0;	// 1 second delay
			
			if(ROM_CODE == read_rom_code_from_ds18b20(&G_rom_code[1][0]))
			{
				change_display_value_to("Upd",DOT_OFF);
			}
			else if(ROM_CODE != read_rom_code_from_ds18b20(&G_rom_code[1][0]))
			{
				change_display_value_to("Err",DOT_OFF);
			}
			write_rom_code_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case SENSOR_3_CONFIG:
			G_menu_state_machine	=	READ_SENSOR_3_ROM;
			change_display_value_to("pro",DOT_OFF);
			
			G_temp_disp_tmr		= 	TAU2_1_SEC;
			uc.temp_disp_delay	=	1;
			while(!uc.call_ds18b20_fun);
			uc.call_ds18b20_fun	=	0;	// 1 second delay
			
			if(ROM_CODE == read_rom_code_from_ds18b20(&G_rom_code[2][0]))
			{
				change_display_value_to("Upd",DOT_OFF);
			}
			else if(ROM_CODE != read_rom_code_from_ds18b20(&G_rom_code[2][0]))
			{
				change_display_value_to("Err",DOT_OFF);
			}
			write_rom_code_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case RUN_MODE:
			T1_LED_OFF;
			T2_LED_OFF;
			T3_LED_OFF;
			G_menu_state_machine	=	DEVICE_ID;
			change_display_value_to(" Id",DOT_OFF);
		break;
		case DEVICE_ID:
			G_menu_state_machine	=	DEVICE_ID_EDIT;
			value_to_edit		=	g_device_id;
			uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			change_display_value_to((char *)data_to_display,DOT_OFF);
		break;
		case SET_POINT:
			G_menu_state_machine	=	SET_POINT_EDIT;
			value_to_edit		=	(int16_t)G_set_point;
			uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			change_display_value_to((char *)data_to_display,DOT_ON);
		break;
		case DEVICE_ID_EDIT:
			G_menu_state_machine	=	DEVICE_ID_UPD;
			g_device_id		=	value_to_edit;
			change_device_id(&g_modbus_settings,g_device_id);
			change_display_value_to("UPd",DOT_OFF);
			write_set_point_offset_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case SET_POINT_EDIT:
			G_menu_state_machine	=	SET_POINT_UPD;
			G_set_point		=	(uint16_t)value_to_edit;
			change_display_value_to("UPd",DOT_OFF);
			write_set_point_offset_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case OFFSET_1:
			G_menu_state_machine	=	OFFSET_1_EDIT;
			value_to_edit		=	(int16_t)G_ds18b20[0].offset;
			check_sign_and_display_offset_value();
			#if 0
			if(value_to_edit < 0)
			{
				uint_to_string(data_to_display,(unsigned int)(value_to_edit * (-1)),3);
				data_to_display[0]	=	'-';
			}
			else
			{
				uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			}
			change_display_value_to((char *)data_to_display,DOT_ON);
			#endif
		break;
		case OFFSET_2:
			G_menu_state_machine	=	OFFSET_2_EDIT;
			value_to_edit		=	(int16_t)G_ds18b20[1].offset;
			check_sign_and_display_offset_value();
		break;
		case OFFSET_3:
			G_menu_state_machine	=	OFFSET_3_EDIT;
			value_to_edit		=	(int16_t)G_ds18b20[2].offset;
			check_sign_and_display_offset_value();
		break;
		case OFFSET_1_EDIT:
			G_menu_state_machine	=	OFFSET_1_UPD;
			G_ds18b20[0].offset	=	(int8_t)value_to_edit;
			change_display_value_to("UPd",DOT_OFF);
			write_set_point_offset_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case OFFSET_2_EDIT:
			G_menu_state_machine	=	OFFSET_2_UPD;
			G_ds18b20[1].offset	=	(int8_t)value_to_edit;
			change_display_value_to("UPd",DOT_OFF);
			write_set_point_offset_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		case OFFSET_3_EDIT:
			G_menu_state_machine	=	OFFSET_3_UPD;
			G_ds18b20[2].offset	=	(int8_t)value_to_edit;
			change_display_value_to("UPd",DOT_OFF);
			write_set_point_offset_to_flash();
			G_display_delay_tmr	= 	D_1000_MS;
			uc.display_delay	=	1;
		break;
		default:
		break;
	}
}

// Handle up key stuffs
void process_up_key_routine(void)
{
	switch(G_menu_state_machine)
	{
		case SENSOR_1_CONFIG:
			G_menu_state_machine	=	SENSOR_2_CONFIG;
	    		change_display_value_to(" t2",DOT_OFF);
			T1_LED_OFF;
			T2_LED_ON;
		break;
		case SENSOR_2_CONFIG:
			G_menu_state_machine	=	SENSOR_3_CONFIG;
	    		change_display_value_to(" t3",DOT_OFF);
			T2_LED_OFF;
			T3_LED_ON;
		break;
		case SENSOR_3_CONFIG:
			G_menu_state_machine	=	SENSOR_1_CONFIG;
	    		change_display_value_to(" t1",DOT_OFF);
			T3_LED_OFF;
			T1_LED_ON;
		break;
		case DEVICE_ID:
			G_menu_state_machine	=	SET_POINT;
			change_display_value_to("SPt",DOT_OFF);
		break;
		case SET_POINT:
			G_menu_state_machine	=	OFFSET_1;
			change_display_value_to("OFS",DOT_OFF);
			T1_LED_ON;T3_LED_OFF;
		break;
		case OFFSET_1:
			G_menu_state_machine	=	OFFSET_2;
			T2_LED_ON;T1_LED_OFF;
		break;
		case OFFSET_2:
			G_menu_state_machine	=	OFFSET_3;
			T3_LED_ON;T2_LED_OFF;
		break;
		case OFFSET_3:
			T3_LED_OFF;
			G_menu_state_machine	=	DEVICE_ID;
			change_display_value_to(" Id",DOT_OFF);
		break;
		case DEVICE_ID_EDIT:
			if(++value_to_edit >= 251)
			{
				value_to_edit	= 1;
			}
			uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			change_display_value_to((char *)data_to_display,DOT_OFF);
			if(uc.key_scan_under_edit)
			{
				G_key_scan_timer	= K_100_MS;
			}
		break;
		case SET_POINT_EDIT:
			if(++value_to_edit >= 351)
			{
				value_to_edit	= 150;
			}
			uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			change_display_value_to((char *)data_to_display,DOT_ON);
			if(uc.key_scan_under_edit)
			{
				G_key_scan_timer	= K_100_MS;
			}
		break;
		case OFFSET_1_EDIT:
		case OFFSET_2_EDIT:
		case OFFSET_3_EDIT:
			if(++value_to_edit >= 51)
			{
				value_to_edit	= -50;
			}
			check_sign_and_display_offset_value();
			#if 0
			if(value_to_edit < 0)
			{
				uint_to_string(data_to_display,(unsigned int)(value_to_edit * (-1)),3);
				data_to_display[0]	=	'-';
			}
			else
			{
				uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			}
			change_display_value_to((char *)data_to_display,DOT_ON);
			#endif
			if(uc.key_scan_under_edit)
			{
				G_key_scan_timer	= K_100_MS;
			}
		break;
		default:
		break;
	}
}

// Handle down key stuffs
void process_down_key_routine(void)
{
	switch(G_menu_state_machine)
	{
		case SENSOR_1_CONFIG:
			G_menu_state_machine	=	SENSOR_3_CONFIG;
	    		change_display_value_to(" t3",DOT_OFF);
			T1_LED_OFF;
			T3_LED_ON;
		break;
		case SENSOR_2_CONFIG:
			G_menu_state_machine	=	SENSOR_1_CONFIG;
	    		change_display_value_to(" t1",DOT_OFF);
			T2_LED_OFF;
			T1_LED_ON;
		break;
		case SENSOR_3_CONFIG:
			G_menu_state_machine	=	SENSOR_2_CONFIG;
	    		change_display_value_to(" t2",DOT_OFF);
			T3_LED_OFF;
			T2_LED_ON;
		break;
		case SET_POINT:
			G_menu_state_machine	=	DEVICE_ID;
			change_display_value_to(" Id",DOT_OFF);
		break;
		case DEVICE_ID:
			G_menu_state_machine	=	OFFSET_3;
			change_display_value_to("OFS",DOT_OFF);
			T3_LED_ON;
		break;
		case OFFSET_1:
			T1_LED_OFF;
			G_menu_state_machine	=	SET_POINT;
			change_display_value_to("SPt",DOT_OFF);
		break;
		case OFFSET_2:
			G_menu_state_machine	=	OFFSET_1;
			T1_LED_ON;T2_LED_OFF;
		break;
		case OFFSET_3:
			G_menu_state_machine	=	OFFSET_2;
			T2_LED_ON;T3_LED_OFF;
		break;
		case DEVICE_ID_EDIT:
			if(--value_to_edit <= 0)
			{
				value_to_edit	= 250;
			}
			uint_to_string(data_to_display,value_to_edit,3);
			change_display_value_to((char *)data_to_display,DOT_OFF);
			if(uc.key_scan_under_edit)
			{
				G_key_scan_timer	= K_100_MS;
			}
		break;
		case SET_POINT_EDIT:
			if(--value_to_edit <= 149)
			{
				value_to_edit	= 350;
			}
			uint_to_string(data_to_display,value_to_edit,3);
			change_display_value_to((char *)data_to_display,DOT_ON);
			if(uc.key_scan_under_edit)
			{
				G_key_scan_timer	= K_100_MS;
			}
		break;
		case OFFSET_1_EDIT:
		case OFFSET_2_EDIT:
		case OFFSET_3_EDIT:
			if(--value_to_edit <= -51)
			{
				value_to_edit	= 50;
			}
			check_sign_and_display_offset_value();
			#if 0
			if(value_to_edit < 0)
			{
				uint_to_string(data_to_display,(unsigned int)(value_to_edit * (-1)),3);
				data_to_display[0]	=	'-';
			}
			else
			{
				uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
			}
			change_display_value_to((char *)data_to_display,DOT_ON);
			#endif
			if(uc.key_scan_under_edit)
			{
				G_key_scan_timer	= K_100_MS;
			}
		break;
		default:
		break;
	}
}

// Handle exit key stuffs
void process_exit_key_routine(void)
{
	uint8_t data_to_display[4];
	switch(G_menu_state_machine)
	{
		case DEVICE_ID:
		case SET_POINT:
		case OFFSET_1:
		case OFFSET_2:
		case OFFSET_3:
			G_menu_state_machine	=	RUN_MODE;
			switch(G_disp_state_on_run_mode)
			{
				case DISPLAY_TEMP_1:
					uint_to_string(data_to_display,G_ds18b20[0].current_temperature,3);
					change_display_value_to((char *)data_to_display,DOT_ON);
					T2_LED_OFF;
					T3_LED_OFF;
					T1_LED_ON;
				break;
				case DISPLAY_TEMP_2:
					uint_to_string(data_to_display,G_ds18b20[1].current_temperature,3);
					change_display_value_to((char *)data_to_display,DOT_ON);
					T1_LED_OFF;
					T3_LED_OFF;
					T2_LED_ON;
				break;
				case DISPLAY_TEMP_3:
					uint_to_string(data_to_display,G_ds18b20[2].current_temperature,3);
					change_display_value_to((char *)data_to_display,DOT_ON);
					T1_LED_OFF;
					T2_LED_OFF;
					T3_LED_ON;
				break;
				default:
					change_display_value_to("---",DOT_OFF);
				break;
				
			}
		break;
		case DEVICE_ID_EDIT:
			G_menu_state_machine	=	DEVICE_ID;
			change_display_value_to(" Id",DOT_OFF);
		break;
		case SET_POINT_EDIT:
			G_menu_state_machine	=	SET_POINT;
			change_display_value_to("SPt",DOT_OFF);
		break;
		case OFFSET_1_EDIT:
			G_menu_state_machine	=	OFFSET_1;
			change_display_value_to("OFS",DOT_OFF);
		break;
		case OFFSET_2_EDIT:
			G_menu_state_machine	=	OFFSET_2;
			change_display_value_to("OFS",DOT_OFF);
		break;
		case OFFSET_3_EDIT:
			G_menu_state_machine	=	OFFSET_3;
			change_display_value_to("OFS",DOT_OFF);
		break;
		default:
			if(RELAY==1)
		   	   RELAY_OFF;
		break;
	}
		
}

void handle_display_after_updation(void)
{
	switch(G_menu_state_machine)
	{
		case READ_SENSOR_1_ROM:
			G_menu_state_machine	=	SENSOR_1_CONFIG;
	    		change_display_value_to(" t1",DOT_OFF);
		break;
		case READ_SENSOR_2_ROM:
			G_menu_state_machine	=	SENSOR_2_CONFIG;
	    		change_display_value_to(" t2",DOT_OFF);
		break;
		case READ_SENSOR_3_ROM:
			G_menu_state_machine	=	SENSOR_3_CONFIG;
	    		change_display_value_to(" t3",DOT_OFF);
		break;
		case DEVICE_ID_UPD:
			G_menu_state_machine	=	DEVICE_ID;
			change_display_value_to(" Id",DOT_OFF);
		break;
		case SET_POINT_UPD:
			G_menu_state_machine	=	SET_POINT;
			change_display_value_to("SPt",DOT_OFF);
		break;
		case OFFSET_1_UPD:
			G_menu_state_machine	=	OFFSET_1;
			change_display_value_to("OFS",DOT_OFF);
		break;
		case OFFSET_2_UPD:
			G_menu_state_machine	=	OFFSET_2;
			change_display_value_to("OFS",DOT_OFF);
		break;
		case OFFSET_3_UPD:
			G_menu_state_machine	=	OFFSET_3;
			change_display_value_to("OFS",DOT_OFF);
		break;
		default:
		break;
	}
}

void check_sign_and_display_offset_value(void)
{
	if(value_to_edit < 0)
	{
		uint_to_string(data_to_display,(unsigned int)(value_to_edit * (-1)),3);
		data_to_display[0]	=	'-';
	}
	else
	{
		uint_to_string(data_to_display,(unsigned int)value_to_edit,3);
	}
	change_display_value_to((char *)data_to_display,DOT_ON);
}
