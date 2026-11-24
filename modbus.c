#include "modbus.h"

volatile uint8_t rx_modbus_buffer[RX_BUFFER_SIZE];
volatile uint8_t tx_modbus_buffer[TX_BUFFER_SIZE];
const uint8_t auchCRCHi[] = 
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40 
};

const uint8_t auchCRCLo[] = 
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 
	0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 
	0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 
	0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 
	0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 
	0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 
	0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 
	0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 
	0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 
	0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
	0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 
	0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 
	0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 
	0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 
	0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 
	0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 
	0x41, 0x81, 0x80, 0x40
};

// Initial seetings for MODBUS driver
void init_modbus(MODBUS_SETTINGS *modbus,uint8_t id,volatile uint8_t *ctrl_port,CTRL_PIN_BIT ctrl_pin,volatile uint8_t *tx_en_reg,TX_EN_BIT tx_bit_no,
		volatile uint8_t *rx_en_reg,RX_EN_BIT rx_bit_no,volatile uint8_t *rxd,volatile uint8_t *txd,volatile uint16_t *ssr,volatile uint16_t *smr,
		uint16_t ms_delay_bw_frames,uint8_t no_of_modules)
{
	modbus->device_id		= id;
	modbus->ctrl_port		= ctrl_port;
	modbus->ctrl_pin		= ctrl_pin;
	modbus->tx_intr_en_reg		= tx_en_reg;
	modbus->rx_intr_en_reg		= rx_en_reg;
	modbus->tx_data			= txd;
	modbus->rx_data			= rxd;
	modbus->ssr_reg			= ssr;
	modbus->smr_reg			= smr;
	modbus->tx_intr_en_pin		= tx_bit_no;
	modbus->rx_intr_en_pin		= rx_bit_no;
	modbus->delay_between_frames	= ms_delay_bw_frames;
	modbus->total_modules		= no_of_modules;
}

void change_device_id(MODBUS_SETTINGS *modbus,uint8_t id)
{
	modbus->device_id		= id;
}

/* Function used to create MODBUS module.
   
   If more than one modules are to be configured, define array of MODBUS_MODULE structure.	*/
   
void create_modbus_module(MODBUS_SETTINGS *modbus,MODBUS_MODULE *module,FUNCTION_CODE func_code,uint16_t start_add,uint16_t total_reg,uint16_t *tx_buff,
			uint16_t *rx_buff,USER_CALL_BACK rx_end)
{
	if(modbus->no_of_modules_configured < modbus->total_modules)
	{
		module->function_code			= func_code;
		switch(func_code)
		{
			case READ_HOLDING_REGISTER:
				module->writable_function_code	= PRESET_SINGLE_REGISTER;
			break;
			case READ_COIL_STATUS:
				module->writable_function_code	= PRESET_SINGLE_REGISTER;
			break;
			default:
				module->writable_function_code	= NO_WRITABLE_FUNCTION;
			break;
		}
		module->start_reg._16_bit_value		= start_add;
		module->no_of_reg._16_bit_value		= total_reg;
		module->tx_data_buff			= tx_buff;
		module->rx_data_buff			= rx_buff;
		module->rx_receive_end			= rx_end;
		modbus->no_of_modules_configured++;
	}
}

// Function used to start all MODBUS module
void start_modbus_module(MODBUS_SETTINGS *modbus,MODBUS_MODULE *module)
{
	uint8_t index;
	modbus->module_add	= (MODBUS_MODULE **)malloc(modbus->no_of_modules_configured * sizeof(MODBUS_MODULE *));
	if(modbus->module_add != NULL)
	{
		for(index = 0; index < modbus->no_of_modules_configured; index++)
		{
			*(modbus->module_add + index)	= &module[index];
		}
		modbus->rx_count	= 0;
		modbus->rx_length	= 8;
		modbus->rx_buff		= rx_modbus_buffer;
		modbus->tx_buff		= tx_modbus_buffer;
		modbus->modbus_status	= START_MODBUS_DATA_RECEIVE;
		modbus->delay_between_frames_counter = modbus->delay_between_frames;
		*(modbus->ctrl_port)	&= (~modbus->ctrl_pin);
		*(modbus->rx_intr_en_reg)&= (~modbus->rx_intr_en_pin);
		*(modbus->tx_intr_en_reg)|= modbus->tx_intr_en_pin;
	}
}

// This function used to process the query and form and send response packet
void process_rx_data(MODBUS_SETTINGS *modbus)
{
	uint16_t crc_value,start_reg,no_of_reg,index;
	uint8_t call_user_rx = 0;FUNCTION_CODE point_type;
	uint8_t remainder;
	crc_value	= crc16((uint8_t *)modbus->rx_buff,(modbus->rx_length - 2));
	if(((crc_value >> 8) == modbus->rx_buff[modbus->rx_length - 2]) && ((crc_value & 0x00FF) == modbus->rx_buff[modbus->rx_length - 1]))
	{
		start_reg	= (modbus->rx_buff[2] << 8) + modbus->rx_buff[3];
		no_of_reg	= (modbus->rx_buff[4] << 8) + modbus->rx_buff[5];
		switch(modbus->rx_buff[1])
		{
#ifdef ENABLE_READ_HOLDING_REGISTER
			case READ_HOLDING_REGISTER:
#endif
#ifdef ENABLE_READ_INPUT_REGISTER
			case READ_INPUT_REGISTER:
#endif
#if (defined(ENABLE_READ_HOLDING_REGISTER) || defined(ENABLE_READ_INPUT_REGISTER))
				if((start_reg + no_of_reg) <= ((modbus->current_frame_module_add)->start_reg._16_bit_value + (modbus->current_frame_module_add)->no_of_reg._16_bit_value))
				{
					modbus->tx_buff[0]	= modbus->device_id;
					modbus->tx_buff[1]	= modbus->rx_buff[1];
					modbus->tx_buff[2]	= no_of_reg * 2;
					for(index = 0; index < modbus->tx_buff[2]; index++)
					{
						modbus->tx_buff[3 + index]	= (modbus->current_frame_module_add)->tx_data_buff[start_reg - (modbus->current_frame_module_add)->start_reg._16_bit_value] >> 8;
						index++;
						modbus->tx_buff[3 + index]	= (modbus->current_frame_module_add)->tx_data_buff[start_reg - (modbus->current_frame_module_add)->start_reg._16_bit_value];
						start_reg++;
					}
					crc_value					= crc16((uint8_t *)modbus->tx_buff,(modbus->tx_buff[2] + 3));
					modbus->tx_buff[3 + modbus->tx_buff[2]]		= crc_value >> 8;
					modbus->tx_buff[4 + modbus->tx_buff[2]]		= crc_value;
					modbus->tx_length				= modbus->tx_buff[2] + 5;
					modbus->delay_between_frames_counter		= modbus->delay_between_frames * 10;
					modbus->modbus_status				= START_MODBUS_DATA_TRANSMIT;
				}
			break;
#endif
#ifdef ENABLE_READ_INPUT_STATUS
			case READ_INPUT_STATUS:
				if((start_reg + no_of_reg) <= ((modbus->current_frame_module_add)->start_reg._16_bit_value + (modbus->current_frame_module_add)->no_of_reg._16_bit_value))
				{
					modbus->tx_buff[0]	= modbus->device_id;
					modbus->tx_buff[1]	= modbus->rx_buff[1];
					modbus->tx_buff[2]	= (no_of_reg / 8) + ((no_of_reg % 8) > 0);
					if(!modbus->tx_buff[2])
					modbus->tx_buff[2]	= 1;
					start_reg		= start_reg - (modbus->current_frame_module_add)->start_reg._16_bit_value;
					remainder		= start_reg % 16;
					start_reg		= start_reg / 16;
					for(index = 0; index < modbus->tx_buff[2]; index++)
					{
						modbus->tx_buff[3 + index]	= (modbus->current_frame_module_add)->tx_data_buff[start_reg] >> remainder;
						if(remainder >= 8)
						{
							if(remainder != 8)
							{
								remainder			= 24 - remainder;
								modbus->tx_buff[3 + index]	|= ((modbus->current_frame_module_add)->tx_data_buff[++start_reg] << (24 - remainder));
								remainder			=  16 - remainder;
							}
							else
							{
								remainder	= 0;
								start_reg++;
							}
						}
						else
						remainder	+= 8;
					}
					crc_value					= crc16((uint8_t *)modbus->tx_buff,(modbus->tx_buff[2] + 3));
					modbus->tx_buff[3 + modbus->tx_buff[2]]		= crc_value >> 8;
					modbus->tx_buff[4 + modbus->tx_buff[2]]		= crc_value;
					modbus->tx_length				= modbus->tx_buff[2] + 5;
					modbus->delay_between_frames_counter		= modbus->delay_between_frames * 10;
					modbus->modbus_status				= START_MODBUS_DATA_TRANSMIT;
				}
			break;
#endif
#ifdef ENABLE_PRESET_SINGLE_REGISTER
			case PRESET_SINGLE_REGISTER:
				if((modbus->current_frame_module_add)->rx_data_buff != NULL)
				{
					call_user_rx					    = 1;
					*((modbus->current_frame_module_add)->rx_data_buff) = no_of_reg;
					no_of_reg					    = 1;
					point_type					    = PRESET_SINGLE_REGISTER;
				}
				memcpy((uint8_t *)modbus->tx_buff,(uint8_t *)modbus->rx_buff,8);
				modbus->tx_length				= 8;
				modbus->delay_between_frames_counter		= modbus->delay_between_frames * 10;
				modbus->modbus_status				= START_MODBUS_DATA_TRANSMIT;
			break;
#endif
			default:
			break;
		}
	}
	
	if(modbus->modbus_status != START_MODBUS_DATA_TRANSMIT)
	{
		process_tx_end(modbus);
	}
	
	if(call_user_rx)
	{
		if((modbus->current_frame_module_add)->rx_receive_end != NULL)
		(modbus->current_frame_module_add)->rx_receive_end(start_reg,point_type,no_of_reg);
	}
}

// This function used to send the response
void process_tx_end(MODBUS_SETTINGS *modbus)
{
	modbus->rx_count	= 0;
	modbus->rx_length	= 8;
	modbus->modbus_status	= START_MODBUS_DATA_RECEIVE;
	modbus->delay_between_frames_counter = modbus->delay_between_frames;
	*(modbus->ctrl_port)	&= (~modbus->ctrl_pin);
	*(modbus->rx_intr_en_reg)&= (~modbus->rx_intr_en_pin);
	*(modbus->tx_intr_en_reg)|= modbus->tx_intr_en_pin;
}

/* This function will check if query frame is received and if it is received it call the function which is process the query.
   And it will call the function which has to be called at end of response completely sent to master.
   
   CALL THIS FUNCTION INSIDE INFINITE LOOP WHILE(1). */
void handle_modbus_received_frame(MODBUS_SETTINGS *modbus)
{
	if(modbus->func_to_call == CALL_RX_END_DRIVER)
	{
		process_rx_data(modbus);
		modbus->func_to_call	= NO_CALL;
	}
	else if(modbus->func_to_call == CALL_TX_END_DRIVER)
	{
		process_tx_end(modbus);
		modbus->func_to_call	= NO_CALL;
	}
}


/* Call this function inside ISR of UART receive function */
void modbus_rx_isr(MODBUS_SETTINGS *modbus)
{
	uint8_t volatile index,rx_data;
	uint16_t start_reg;
	rx_data	= *(modbus->rx_data);
	modbus->delay_between_frames_counter = modbus->delay_between_frames;
	if(modbus->rx_count < modbus->rx_length)
	{
		*(modbus->rx_buff + modbus->rx_count)	= rx_data;
		switch(modbus->rx_count)
		{
			case 0:
				if(*(modbus->rx_buff) == modbus->device_id)
				{
					modbus->rx_count++;
					modbus->modbus_status 	= MODBUS_DATA_RECEIVING;
				}
			break;
			case 1:
				for(index = 0; index < modbus->no_of_modules_configured; index++)
				{
					if(*(modbus->rx_buff + 1) == (*(modbus->module_add + index))->function_code)
					{
						modbus->rx_count++;
						break;
					}
					else if(*(modbus->rx_buff + 1) == (*(modbus->module_add + index))->writable_function_code)
					{
						modbus->rx_count++;
						break;
					}
				}
				if(index == modbus->no_of_modules_configured)
				{
					modbus->rx_count	= 0;
					modbus->modbus_status	= START_MODBUS_DATA_RECEIVE;
				}
			break;
			case 3:
				start_reg				= ((*(modbus->rx_buff + 2)) << 8) + *(modbus->rx_buff + 3);
				modbus->current_frame_module_add	= NULL;
				for(index = 0; index < modbus->no_of_modules_configured; index++)
				{
					if((*(modbus->module_add + index))->start_reg._16_bit_value <= start_reg && 
					   ((*(modbus->module_add + index))->start_reg._16_bit_value + (*(modbus->module_add + index))->no_of_reg._16_bit_value) >= start_reg)
					{
						if((*(modbus->module_add + index))->function_code == *(modbus->rx_buff + 1) ||
						   (*(modbus->module_add + index))->writable_function_code == *(modbus->rx_buff + 1))
						{
							modbus->current_frame_module_add	= *(modbus->module_add + index);
							modbus->rx_count++;
							break;
						}
					}
				}
				if(modbus->current_frame_module_add == NULL)
				{
					modbus->rx_count	= 0;
					modbus->modbus_status	= START_MODBUS_DATA_RECEIVE;
				}
			break;
			default:
				modbus->rx_count++;
			break;
		}
		if(modbus->rx_count == modbus->rx_length)
		{
			modbus->modbus_status		= MODBUS_IDLE;
			*(modbus->rx_intr_en_reg)	|= modbus->rx_intr_en_pin;
			modbus->func_to_call		= CALL_RX_END_DRIVER;
		}
	}
}

/* Call this function inside ISR of UART transmit function */
void modbus_tx_isr(MODBUS_SETTINGS *modbus)
{
	if(modbus->tx_count < modbus->tx_length)
	{
		*(modbus->tx_data)		= modbus->tx_buff[modbus->tx_count++];
	}
	else
	{
		*(modbus->smr_reg) &= ~0x0001U;

	        if ((*(modbus->ssr_reg) & 0x0040U) == 0U)
	        {
	            modbus->func_to_call	= CALL_TX_END_DRIVER;
	        }
	}
}

/* Call this function inside 1msec timer ISR */
void modbus_1_msec_isr(MODBUS_SETTINGS *modbus)
{
	if(modbus->modbus_status == START_MODBUS_DATA_TRANSMIT)
	{
		if((modbus->delay_between_frames_counter)-- == 0)
		{
			*(modbus->tx_data)		= modbus->tx_buff[0];
			modbus->tx_count		= 1;
			*(modbus->ctrl_port)		|= modbus->ctrl_pin;
			*(modbus->tx_intr_en_reg)	&= (~modbus->tx_intr_en_pin);
			modbus->modbus_status		= MODBUS_DATA_TRANSMITTING;
			modbus->delay_between_frames_counter	= modbus->delay_between_frames;
		}
	}
	else if(modbus->modbus_status == MODBUS_DATA_RECEIVING)
	{
		if((modbus->delay_between_frames_counter)-- == 0)
		{
			modbus->delay_between_frames_counter	= modbus->delay_between_frames;
			modbus->rx_count			= 0;
			modbus->rx_length			= 8;
			modbus->modbus_status			= START_MODBUS_DATA_RECEIVE;
		}
	}
}

/***********************************************************************************************************************
* Function Name: crc16
* Description  : This function is used to calculate CRC
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen) 
{
	uint8_t uchCRCHi = 0xFF; 		// high byte of CRC initialized
	uint8_t uchCRCLo = 0xFF; 		// low byte of CRC initialized
	uint16_t final;
	uint16_t uIndex; 					
	
	while(usDataLen--)
	{
		uIndex   = uchCRCHi ^ *puchMsg++; 	// calculate the CRC
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	
	final  = uchCRCHi;
	final  = final << 8;
	final |= uchCRCLo;
	
	return final;
}