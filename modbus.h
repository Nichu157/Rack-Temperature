#ifndef _MODBUS_
#define _MODBUS_
#include "r_cg_macrodriver.h"
#include "stdlib.h"
#include "string.h"

#define ENABLE_READ_HOLDING_REGISTER
#define ENABLE_READ_INPUT_REGISTER
#define ENABLE_READ_INPUT_STATUS
#define ENABLE_PRESET_SINGLE_REGISTER

#define RX_BUFFER_SIZE		10
#define TX_BUFFER_SIZE		20

enum bit_position
{
	BIT0	= (1U << 0),
	BIT1	= (1U << 1),
	BIT2	= (1U << 2),
	BIT3	= (1U << 3),
	BIT4	= (1U << 4),
	BIT5	= (1U << 5),
	BIT6	= (1U << 6),
	BIT7	= (1U << 7)
};

typedef enum
{
	NO_WRITABLE_FUNCTION,
	READ_COIL_STATUS,
	READ_INPUT_STATUS,
	READ_HOLDING_REGISTER,
	READ_INPUT_REGISTER,
	FORCE_SINGLE_COIL,
	PRESET_SINGLE_REGISTER
}FUNCTION_CODE;

typedef enum
{
	NO_CALL,
	CALL_RX_END_DRIVER,
	CALL_TX_END_DRIVER
}MOD_DRI_FUN_CALLER;

typedef enum
{
	MODBUS_IDLE,
	START_MODBUS_DATA_RECEIVE,
	START_MODBUS_DATA_TRANSMIT,
	MODBUS_DATA_TRANSMITTING,
	MODBUS_DATA_RECEIVING
}MODBUS_STATUS;

typedef enum bit_position CTRL_PIN_BIT;
typedef enum bit_position TX_EN_BIT;
typedef enum bit_position RX_EN_BIT;
typedef void (__near *USER_CALL_BACK)(uint16_t, FUNCTION_CODE, uint8_t);

typedef volatile struct
{
	FUNCTION_CODE function_code;
	FUNCTION_CODE writable_function_code;
	union{
		uint16_t _16_bit_value;
		struct{
			uint8_t ls_byte;
			uint8_t ms_byte;
		}byte;
	}start_reg,no_of_reg;
	uint16_t *tx_data_buff;
	uint16_t *rx_data_buff;
	uint8_t byte_count;
	USER_CALL_BACK rx_receive_end;
}MODBUS_MODULE;

typedef volatile struct
{
	uint8_t device_id;
	volatile uint8_t *ctrl_port;
	CTRL_PIN_BIT ctrl_pin;
	volatile uint8_t *tx_intr_en_reg;
	volatile uint8_t *rx_intr_en_reg;
	volatile uint8_t *tx_data;
	volatile uint8_t *rx_data;
	volatile uint16_t *smr_reg,*ssr_reg;
	volatile uint8_t *rx_buff,*tx_buff;
	TX_EN_BIT tx_intr_en_pin;
	RX_EN_BIT rx_intr_en_pin;
	uint16_t delay_between_frames;
	uint16_t delay_between_frames_counter;
	MODBUS_STATUS modbus_status;
	uint8_t total_modules;
	uint8_t no_of_modules_configured;
	uint8_t rx_count,rx_length;
	uint8_t tx_count,tx_length;
	MOD_DRI_FUN_CALLER func_to_call;
	MODBUS_MODULE **module_add;
	MODBUS_MODULE *current_frame_module_add;
}MODBUS_SETTINGS;

void init_modbus(MODBUS_SETTINGS *,uint8_t ,volatile uint8_t *,CTRL_PIN_BIT ,volatile uint8_t *,TX_EN_BIT ,
		volatile uint8_t *,RX_EN_BIT ,volatile uint8_t * ,volatile uint8_t *,volatile uint16_t *,volatile uint16_t *, uint16_t ,uint8_t);
void create_modbus_module(MODBUS_SETTINGS *,MODBUS_MODULE *,FUNCTION_CODE ,uint16_t ,uint16_t ,uint16_t *,uint16_t *,USER_CALL_BACK );
void start_modbus_module(MODBUS_SETTINGS *,MODBUS_MODULE *);
void process_rx_data(MODBUS_SETTINGS *);
void process_tx_end(MODBUS_SETTINGS *);
void handle_modbus_received_frame(MODBUS_SETTINGS *);
void modbus_rx_isr(MODBUS_SETTINGS *);
void modbus_tx_isr(MODBUS_SETTINGS *);
void modbus_1_msec_isr(MODBUS_SETTINGS *);
uint16_t crc16(uint8_t *, uint16_t );
void change_device_id(MODBUS_SETTINGS *modbus,uint8_t id);

#endif