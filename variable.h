#ifndef _VARIABLE_

#define _VARIABLE_

typedef volatile union 
{
	struct 
	{
		 uint8_t  bit0 	 : 1;
		 uint8_t  bit1 	 : 1;
		 uint8_t  bit2 	 : 1;
		 uint8_t  bit3 	 : 1;
		 uint8_t  bit4 	 : 1;
		 uint8_t  bit5 	 : 1;
		 uint8_t  bit6 	 : 1;
		 uint8_t  bit7 	 : 1;
	}bit_f;
	uint8_t  byte;
}BYTE_TO_BITFIELD;

typedef volatile struct
{
	uint8_t key_scan_under_edit		: 1;
	uint8_t call_key_routine		: 1;
	uint8_t rom_search			: 1;
	uint8_t start_up_delay			: 1;
	uint8_t display_delay			: 1;
	uint8_t value_updated			: 1;
	uint8_t temp_disp_delay			: 1;
	uint8_t call_ds18b20_fun		: 1;
	uint8_t t1_alarm			: 1;
	uint8_t t2_alarm			: 1;
	uint8_t t3_alarm			: 1;
}BITFIELD;

#endif