#include "string.h"
#include "r_cg_timer.h"
#include "user_io_define.h"
#include "ds18b20.h"
#include "display.h"
#include "keypad.h"
#include "modbus_user.h"


static const uint8_t w1CRC[] = {
	
	0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
	175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
	50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
	202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
	87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
	233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
	116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53

};

unsigned char w1_reset(void)
{
  unsigned char err;

  CHANNEL_DIR_OUT;
  CHANNEL = LOW;
  DelayUS(480);
  CHANNEL_DIR_IN;
  DelayUS(80);
  err=CHANNEL;
  DelayUS(400);

  return err;
}

unsigned char w1_bit_io(unsigned char b)
{
	CHANNEL = LOW;
	CHANNEL_DIR_OUT;
	DelayUS(2);
	if(b)
	CHANNEL_DIR_IN;
	DelayUS(5);
	b=CHANNEL;
	DelayUS(90);
	CHANNEL_DIR_IN;
	DelayUS(2);

	return b;
}

unsigned char w1_byte_wr(unsigned char b)
{
  	unsigned char i = 8, j;
  	do{
    	j = w1_bit_io( b & 1 );
    	b >>= 1;
    	if( j )
      		b |= 0x80;
  	}while( --i );
  	return b;
}

unsigned char w1_byte_rd(void)
{
  	return w1_byte_wr(0xFF);
}
unsigned char w1_rom_search( unsigned char diff, unsigned char *id )
{
 	unsigned char i, j, next_diff;
  	unsigned char b;

  	if( w1_reset() )
    	return PRESENCE_ERR;			// error, no device found
  	w1_byte_wr( SEARCH_ROM );			// ROM search command
  	next_diff = LAST_DEVICE;			// unchanged on last device
  	i = 8 * 8;					// 8 bytes
  	do{
    	j = 8;					// 8 bits
    	do{
    		b = w1_bit_io( 1 );			// read bit
      		if( w1_bit_io( 1 ) ){			// read complement bit
			if( b )					// 11
	  			return DATA_ERR;			// data error
      		}
		else{
			if( !b ){				// 00 = 2 devices
	  			if( diff > i || ((*id & 1) && diff != i) ){
	    			b = 1;				// now 1
	    			next_diff = i;			// next pass 0
	  		}
		}
      	}
      	w1_bit_io( b );     			// write bit
      	*id >>= 1;
      	if( b )					// store bit
		*id |= 0x80;
      	i--;
    	}while( --j );
    	id++;					// next byte
  	}while(i);
  	return next_diff;				// to continue search
}

#if 0
void w1_command( unsigned char command, unsigned char *id )
{
  	unsigned char i;					//uchar i;
  	w1_reset();
  	if(id) {
    	w1_byte_wr(MATCH_ROM);			// to a single device
    	i=8;
    	do {
      		w1_byte_wr(*id);
      		id++;
    	}while(--i);
  	}
	else {
  		w1_byte_wr(SKIP_ROM);			// to all devices
  	}
  	w1_byte_wr(command);
}
#endif

//-----------------------------------------------------------------------
// calculate CRC for 1 wire bus
//-----------------------------------------------------------------------
unsigned char w1_byte_crc( unsigned char *puchMsg, unsigned char usDataLen) 
{
    unsigned char uchCRC = 0x00; 				//CRC initialized
    static unsigned char final;
    unsigned uIndex ; 							// will index into CRC lookup table
    while(usDataLen--){
        uIndex = uchCRC ^ *puchMsg++ ; 			// calculate the CRC
        uchCRC = w1CRC[uIndex];
    }
    final=uchCRC;
    return (final);
}

//-----------------------------------------------------------------------
// 5usec delay routine 
//-----------------------------------------------------------------------
void DelayUS(unsigned int microSec) 
{
	G_usec5 = microSec/5;
	R_TAU0_Channel3_Start();
	while(G_usec5);
	#if 0
	{
		NOP();
	}
	#endif
}

/***********************************************************/
// 			Read rom code from ds18b20					   //
/***********************************************************/
unsigned char read_rom_code_from_ds18b20(volatile uint8_t *data)
{
	unsigned char diff,ret_data;
	#if 0
	for(i = 0; i <= 7; i++)
	{
		*data	= 0x00;
	}
	#endif
	memset((void *)data,0,10);
	G_rom_search_timer	= 0;
	uc.rom_search		= 1;
	for(diff = SEARCH_FIRST; diff != LAST_DEVICE; )
	{
		diff = w1_rom_search(diff,(uint8_t *)data);				// Read rom code
		if(!uc.rom_search)
		{
			diff	= PRESENCE_ERR;
			break;
		}
	}
	if(*data	== FAMILY_CODE)
	{
		if(*(data + 7) == w1_byte_crc((uint8_t *)data,7))
		{
			if(diff == LAST_DEVICE)
			{
				ret_data	= ROM_CODE;
			}
			else
			{
				ret_data	= DATA_ERR;
			}
		}
		else
		{
			ret_data	= DATA_ERR;
		}
	}
	else
	{
		ret_data	= DATA_ERR;
	}
	return ret_data;
}

/************************************************************
*	Initiate temperature conversion
*************************************************************/
unsigned char initiate_conversion(volatile uint8_t *rom_code)
{
	unsigned char ret_data,byte;
	
	if(*rom_code == FAMILY_CODE)
	{
		if(0 == w1_reset())
		{
			w1_byte_wr(MATCH_ROM);
			for(byte = 0; byte <= 7; byte++)
			{
				w1_byte_wr(*rom_code++);
			}
			w1_byte_wr(CONVERT_T);
			
			ret_data	= CONV_INITIATED;
		}
		else
		{
			ret_data	= PRESENCE_ERR;
		}
	}
	else 
	{
		ret_data	= NO_FAMILY_CODE;
	}
	return ret_data;
}

/***********************************************************/
// 			Read temperature from ds18b20	   //
/***********************************************************/
unsigned char read_temp_from_ds18b20(volatile uint8_t *rom_code)
{
	unsigned char ret_data;
	unsigned char i;
	static unsigned char data[10];
	unsigned int temperature;
	unsigned char crc_count = 0;
	float temp_float;
	volatile uint8_t *rom_data;
	
	do
	{
		rom_data	=	rom_code;
		if(0 == w1_reset())
		{
			w1_byte_wr(MATCH_ROM);
			for(i = 0; i <= 7; i++)
			{
				w1_byte_wr(*rom_data++);
			}
			w1_byte_wr(READ);
			
			for(i = 0; i <= 8; i++)
			{
				data[i]		= w1_byte_rd();
			}
			crc_count++;
			if(crc_count >= 5)
			{
				if(data[8] != w1_byte_crc(data,8))
				{
					ret_data	= PRESENCE_ERR;
					break;
				}
			}
		}
		else
		{
			ret_data	= PRESENCE_ERR;
			break;
		}
	}while(data[8] != w1_byte_crc(data,8));
	if(data[8]	== w1_byte_crc(data,8))
	{
		data[1]	   		&= 0x87;					// Read sign bit and LSB 4 bit
		temperature		 = data[1];
		temperature	   	 <<= 8;
		temperature		 += data[0];
		temp_float	 	 = temperature * 0.0625;		// 12 bit  ADC
		G_temperature		 = (temp_float + 0.05f) * 10;
		if(G_temperature == 850)
		{
			if(uc.start_up_delay)
			{
				G_temperature	= 0xFFFE;
			}
		}
		ret_data		 = TEMP_READ_END;
	}
	
	return ret_data;
}


void handle_ds18b20(void)
{
	static uint8_t sensor_1_read_fail_count,sensor_2_read_fail_count,sensor_3_read_fail_count;
	uint8_t data_to_display[4],init_status;
	switch(G_ds18b20_state)
	{
		case INIT_SENSOR_1:
			init_status	=	initiate_conversion(&G_rom_code[0][0]);
			if(CONV_INITIATED == init_status)
			{
				G_ds18b20_state		= 	SENSOR_1_INITED;
				G_temp_disp_tmr		=	TAU2_1_SEC;
				uc.temp_disp_delay	=	1;
			}
			else if(init_status == PRESENCE_ERR)
			{
				#if 0
				change_display_value_to("Err",DOT_OFF);
				T3_LED_OFF;
				T1_LED_ON;
				#endif
				G_ds18b20_state			= 	SENSOR_1_ERR;
				G_temp_disp_tmr			=	TAU2_2_SEC;
				uc.temp_disp_delay		=	1;
			}
			else
			{
				G_ds18b20_state		= 	INIT_SENSOR_2;
				uc.call_ds18b20_fun	=	1;
			}
		break;
		case INIT_SENSOR_2:
			init_status	=	initiate_conversion(&G_rom_code[1][0]);
			if(CONV_INITIATED == init_status)
			{
				G_ds18b20_state			=	SENSOR_2_INITED;
				G_temp_disp_tmr			=	TAU2_1_SEC;
				uc.temp_disp_delay		=	1;
			}
			else if(init_status == PRESENCE_ERR)
			{
				#if 0
				change_display_value_to("Err",DOT_OFF);
				T1_LED_OFF;
				T2_LED_ON;
				#endif
				G_ds18b20_state			= 	SENSOR_2_ERR;
				G_temp_disp_tmr			=	TAU2_2_SEC;
				uc.temp_disp_delay		=	1;
			}
			else
			{
				G_ds18b20_state		= 	INIT_SENSOR_3;
				uc.call_ds18b20_fun	=	1;
			}
		break;
		case INIT_SENSOR_3:
			init_status	=	initiate_conversion(&G_rom_code[2][0]);
			if(CONV_INITIATED == init_status)
			{
				G_ds18b20_state			= 	SENSOR_3_INITED;
				G_temp_disp_tmr			=	TAU2_1_SEC;
				uc.temp_disp_delay		=	1;
			}
			else if(init_status == PRESENCE_ERR)
			{
				#if 0
				change_display_value_to("Err",DOT_OFF);
				T1_LED_OFF;
				T2_LED_ON;
				#endif
				G_ds18b20_state			= 	SENSOR_3_ERR;
				G_temp_disp_tmr			=	TAU2_2_SEC;
				uc.temp_disp_delay		=	1;
			}
			else
			{
				G_ds18b20_state		= 	INIT_SENSOR_1;
				uc.call_ds18b20_fun	=	1;
			}
		break;
		case SENSOR_1_INITED:
			if(1 == w1_bit_io(1))
			{
				if(TEMP_READ_END == read_temp_from_ds18b20(&G_rom_code[0][0]))
				{
					if(G_temperature != 0xFFFE)
					{
						sensor_1_read_fail_count=	0;
						G_ds18b20_state		= 	DISPLAY_TEMP_1;
					}
					else
					{
						G_ds18b20_state			= 	SENSOR_1_ERR;
					}
					G_temp_disp_tmr		=	TAU2_1_SEC;
					uc.temp_disp_delay	=	1;
				}
				else
				{
					if(sensor_1_read_fail_count >= 3)
					{
						G_ds18b20_state		= 	SENSOR_1_ERR;
						G_temp_disp_tmr		=	TAU2_1_SEC;
						uc.temp_disp_delay	=	1;
					}
					else
					{
						sensor_1_read_fail_count++;
						G_ds18b20_state		= 	INIT_SENSOR_2;
						uc.call_ds18b20_fun	=	1;
					}
				}
			}
			else
			{
				if(sensor_1_read_fail_count >= 3)
				{
					G_ds18b20_state		= 	SENSOR_1_ERR;
					G_temp_disp_tmr		=	TAU2_1_SEC;
					uc.temp_disp_delay	=	1;
				}
				else
				{
					sensor_1_read_fail_count++;
					G_ds18b20_state		= 	INIT_SENSOR_2;
					uc.call_ds18b20_fun	=	1;
				}
			}
		break;
		case SENSOR_2_INITED:
			if(1 == w1_bit_io(1))
			{
				if(TEMP_READ_END == read_temp_from_ds18b20(&G_rom_code[1][0]))
				{
					if(G_temperature != 0xFFFE)
					{
						sensor_2_read_fail_count=	0;
						G_ds18b20_state		= 	DISPLAY_TEMP_2;
					}
					else
					{
						G_ds18b20_state		= 	SENSOR_2_ERR;
					}
					G_temp_disp_tmr		=	TAU2_1_SEC;
					uc.temp_disp_delay	=	1;
				}
				else
				{
					if(sensor_2_read_fail_count >= 3)
					{
						G_ds18b20_state		= 	SENSOR_2_ERR;
						G_temp_disp_tmr		=	TAU2_1_SEC;
						uc.temp_disp_delay	=	1;
					}
					else
					{
						sensor_2_read_fail_count++;
						G_ds18b20_state		= 	INIT_SENSOR_3;
						uc.call_ds18b20_fun	=	1;
					}
				}
			}
			else
			{
				if(sensor_2_read_fail_count >= 3)
				{
					G_ds18b20_state		= 	SENSOR_2_ERR;
					G_temp_disp_tmr		=	TAU2_1_SEC;
					uc.temp_disp_delay	=	1;
				}
				else
				{
					sensor_2_read_fail_count++;
					G_ds18b20_state		= 	INIT_SENSOR_3;
					uc.call_ds18b20_fun	=	1;
				}
			}
			
		break;
		case SENSOR_3_INITED:
			if(1 == w1_bit_io(1))
			{
				if(TEMP_READ_END == read_temp_from_ds18b20(&G_rom_code[2][0]))
				{
					if(G_temperature != 0xFFFE)
					{
						sensor_3_read_fail_count=	0;
						G_ds18b20_state		= 	DISPLAY_TEMP_3;
					}
					else
					{
						G_ds18b20_state		= 	SENSOR_3_ERR;
					}
					G_temp_disp_tmr		=	TAU2_1_SEC;
					uc.temp_disp_delay	=	1;
				}
				else
				{
					if(sensor_3_read_fail_count >= 3)
					{
						G_ds18b20_state		= 	SENSOR_3_ERR;
						G_temp_disp_tmr		=	TAU2_1_SEC;
						uc.temp_disp_delay	=	1;
					}
					else
					{
						sensor_3_read_fail_count++;
						G_ds18b20_state		= 	INIT_SENSOR_1;
						uc.call_ds18b20_fun	=	1;
					}
				}
			}
			else
			{
				if(sensor_3_read_fail_count >= 3)
				{
					G_ds18b20_state		= 	SENSOR_3_ERR;
					G_temp_disp_tmr		=	TAU2_1_SEC;
					uc.temp_disp_delay	=	1;
				}
				else
				{
					sensor_3_read_fail_count++;
					G_ds18b20_state		= 	INIT_SENSOR_1;
					uc.call_ds18b20_fun	=	1;
				}
			}
		break;
		case SENSOR_1_ERR:
			G_disp_state_on_run_mode	=	SENSOR_1_ERR;
			G_ds18b20[0].current_temperature=	G_temperature;
			g_temp_value_modbus[0]		=	0;
			g_alarm_err_status		|=	TEMP_1_ERROR;
			if(G_menu_state_machine == RUN_MODE)
			{
				change_display_value_to("Err",DOT_OFF);
				T2_LED_OFF;
				T3_LED_OFF;
				T1_LED_ON;
			}
			G_ds18b20_state		= 	INIT_SENSOR_2;
			uc.call_ds18b20_fun	=	1;
		break;
		case SENSOR_2_ERR:
			G_disp_state_on_run_mode	=	SENSOR_2_ERR;
			G_ds18b20[1].current_temperature=	G_temperature;
			g_temp_value_modbus[1]		=	0;
			g_alarm_err_status		|=	TEMP_2_ERROR;
			if(G_menu_state_machine == RUN_MODE)
			{
				change_display_value_to("Err",DOT_OFF);
				T1_LED_OFF;
				T3_LED_OFF;
				T2_LED_ON;
			}
			G_ds18b20_state		= 	INIT_SENSOR_3;
			uc.call_ds18b20_fun	=	1;
		break;
		case SENSOR_3_ERR:
			G_disp_state_on_run_mode	=	SENSOR_3_ERR;
			G_ds18b20[2].current_temperature=	G_temperature;
			g_temp_value_modbus[2]		=	0;
			g_alarm_err_status		|=	TEMP_3_ERROR;
			if(G_menu_state_machine == RUN_MODE)
			{
				change_display_value_to("Err",DOT_OFF);
				T1_LED_OFF;
				T2_LED_OFF;
				T3_LED_ON;
			}
			G_ds18b20_state		= 	INIT_SENSOR_1;
			uc.call_ds18b20_fun	=	1;
		break;
		case DISPLAY_TEMP_1:
			G_disp_state_on_run_mode	=	DISPLAY_TEMP_1;
			G_ds18b20[0].current_temperature=	G_temperature + G_ds18b20[0].offset;
			g_temp_value_modbus[0]		= 	G_ds18b20[0].current_temperature;
			g_alarm_err_status		&=	(~TEMP_1_ERROR);
			if(G_ds18b20[0].current_temperature >= G_set_point)
			{
				if(!uc.t1_alarm)
				{
					g_alarm_err_status	|=	TEMP_1_ALARM;
					uc.t1_alarm		=	1;
					RELAY_ON;
				}
			}
			else
			{
				g_alarm_err_status	&=	(~TEMP_1_ALARM);
				if(!uc.t2_alarm && !uc.t3_alarm && uc.t1_alarm)
				{
					RELAY_OFF;
				}
				uc.t1_alarm	=	0;
			}
			
			if(G_menu_state_machine == RUN_MODE)
			{
				uint_to_string(data_to_display,G_ds18b20[0].current_temperature,3);
				change_display_value_to((char *)data_to_display,DOT_ON);
				T2_LED_OFF;
				T3_LED_OFF;
				T1_LED_ON;
			}
			G_ds18b20_state		= 	INIT_SENSOR_2;
			uc.call_ds18b20_fun	=	1;
		break;
		case DISPLAY_TEMP_2:
			G_disp_state_on_run_mode	=	DISPLAY_TEMP_2;
			G_ds18b20[1].current_temperature=	G_temperature + G_ds18b20[1].offset;
			g_temp_value_modbus[1]		= 	G_ds18b20[1].current_temperature;
			g_alarm_err_status		&=	(~TEMP_2_ERROR);
			if(G_ds18b20[1].current_temperature >= G_set_point)
			{
				if(!uc.t2_alarm)
				{
					g_alarm_err_status	|=	TEMP_2_ALARM;
					uc.t2_alarm		=	1;
					RELAY_ON;
				}
			}
			else
			{
				g_alarm_err_status	&=	(~TEMP_2_ALARM);
				if(!uc.t1_alarm && !uc.t3_alarm && uc.t2_alarm)
				{
					RELAY_OFF;
				}
				uc.t2_alarm	=	0;
			}
			
			if(G_menu_state_machine == RUN_MODE)
			{
				uint_to_string(data_to_display,G_ds18b20[1].current_temperature,3);
				change_display_value_to((char *)data_to_display,DOT_ON);
				T1_LED_OFF;
				T3_LED_OFF;
				T2_LED_ON;
			}
			G_ds18b20_state		= 	INIT_SENSOR_3;
			uc.call_ds18b20_fun	=	1;
		break;	
		case DISPLAY_TEMP_3:
			G_disp_state_on_run_mode	=	DISPLAY_TEMP_3;
			G_ds18b20[2].current_temperature=	G_temperature + G_ds18b20[2].offset;
			g_temp_value_modbus[2]		= 	G_ds18b20[2].current_temperature;
			g_alarm_err_status		&=	(~TEMP_3_ERROR);
			if(G_ds18b20[2].current_temperature >= G_set_point)
			{
				if(!uc.t3_alarm)
				{
					g_alarm_err_status	|=	TEMP_3_ALARM;
					uc.t3_alarm		=	1;
					RELAY_ON;
				}
			}
			else
			{
				g_alarm_err_status	&=	(~TEMP_3_ALARM);
				if(!uc.t1_alarm && !uc.t2_alarm && uc.t3_alarm)
				{
					RELAY_OFF;
				}
				uc.t3_alarm	=	0;
			}
			
			if(G_menu_state_machine == RUN_MODE)
			{
				uint_to_string(data_to_display,G_ds18b20[2].current_temperature,3);
				change_display_value_to((char *)data_to_display,DOT_ON);
				T1_LED_OFF;
				T2_LED_OFF;
				T3_LED_ON;
			}
			G_ds18b20_state		= 	INIT_SENSOR_1;
			uc.call_ds18b20_fun	=	1;
		break;
		default:
		break;
	}
}