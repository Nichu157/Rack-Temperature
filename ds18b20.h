#include "r_cg_macrodriver.h"
#include "variable.h"

#ifndef _1wire_h_
#define _1wire_h_

#define MATCH_ROM	0x55
#define SKIP_ROM	0xCC
#define	SEARCH_ROM	0xF0

#define CONVERT_T	0x44		// DS1820 commands
#define READ		0xBE
#define WRITE		0x4E
#define EE_WRITE	0x48
#define EE_RECALL	0xB8

#define	SEARCH_FIRST    0xFF		// start new search
#define	PRESENCE_ERR	0xFF
#define	DATA_ERR	0xFE
#define LAST_DEVICE	0x00		// last device found

#define ROM_CODE	0xFD
#define TEMP_READ_END	0xFC
#define CONV_INITIATED	0xFB
//			0x01 ... 0x40: continue searching

#define FAMILY_CODE	0x28
#define NO_FAMILY_CODE	0xAA

#define LOW		0
#define HIGH		1

#define CHANNEL_DIR_IN		PM1_bit.no4 = 1;
#define CHANNEL_DIR_OUT		PM1_bit.no4 = 0;
#define CHANNEL			P1_bit.no4

#define DS_1000_MS		100

#define TAU2_1_SEC		10
#define TAU2_2_SEC		20
#define TAU2_3_SEC		30

extern BITFIELD uc;

extern volatile uint8_t G_usec5;
extern volatile uint8_t G_rom_search_timer;
extern volatile uint8_t G_rom_code[3][10];

extern volatile uint16_t G_temperature;

extern volatile uint8_t G_temp_disp_tmr;
extern volatile uint8_t G_disp_state_on_run_mode;

typedef volatile struct
{
	uint16_t current_temperature;
	int8_t  offset;
}SENSOR_DATA;
extern SENSOR_DATA G_ds18b20[3];

extern volatile uint16_t G_set_point;

typedef enum
{
	INIT_SENSOR_1 = 0,
	INIT_SENSOR_2,
	INIT_SENSOR_3,
	SENSOR_1_INITED,
	SENSOR_2_INITED,
	SENSOR_3_INITED,
	SENSOR_1_READ,
	SENSOR_2_READ,
	SENSOR_3_READ,
	SENSOR_1_ERR,
	SENSOR_2_ERR,
	SENSOR_3_ERR,
	DISPLAY_TEMP_1,
	DISPLAY_TEMP_2,
	DISPLAY_TEMP_3
}DS_STATE;

extern volatile DS_STATE G_ds18b20_state;

#if 0
bit w1_reset(void);
uint w1_byte_wr( uchar b );
uint w1_byte_rd( void );
uchar w1_rom_search( uchar diff, uchar idata *id );
void w1_command( uchar command, uchar idata *id );
#endif


unsigned char w1_reset(void);
unsigned char w1_bit_io(unsigned char b);
unsigned char w1_byte_wr(unsigned char b);
unsigned char w1_byte_rd(void);
unsigned char w1_byte_crc( unsigned char *puchMsg, unsigned char usDataLen);
unsigned char w1_rom_search( unsigned char diff, unsigned char *id );

#if 0
void w1_command( unsigned char command, unsigned char *id );
void start_meas(void);
void read_meas(void);
#endif

void DelayUS(unsigned int microSec);
unsigned char read_rom_code_from_ds18b20(volatile uint8_t *data);
void handle_ds18b20(void);

#endif
