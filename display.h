
#include "r_cg_macrodriver.h"
#include "variable.h"

#ifndef _DISPLAY_

#define _DISPLAY_

#define A1			P2_bit.no1
#define B1			P2_bit.no2
#define C1			P2_bit.no3
#define D1			P14_bit.no7
#define E1			P1_bit.no0
#define F1			P1_bit.no1
#define G1			P1_bit.no2
#define DP			P1_bit.no3

#define DISP_1_ON		P5_bit.no1 = 1;
#define DISP_1_OFF		P5_bit.no1 = 0;
#define DISP_2_ON		P5_bit.no0 = 1;
#define DISP_2_OFF		P5_bit.no0 = 0;
#define DISP_3_ON		P3_bit.no0 = 1;
#define DISP_3_OFF		P3_bit.no0 = 0;

#define T1_LED_ON		P2_bit.no0 = 0
#define T1_LED_OFF		P2_bit.no0 = 1
#define T2_LED_ON		P0_bit.no1 = 0
#define T2_LED_OFF		P0_bit.no1 = 1
#define T3_LED_ON		P6_bit.no0 = 0
#define T3_LED_OFF		P6_bit.no0 = 1

#define DOT_ON			1
#define DOT_OFF			0

#define D_1000_MS		100

// Global variables
extern BYTE_TO_BITFIELD seven_seg_led;
extern volatile uint8_t G_display_data[4];
extern volatile uint8_t G_display_delay_tmr;

// Function declarations
unsigned char find_7_segment_value(unsigned char data);
unsigned char uint_to_string(unsigned char *add,unsigned int data,uint8_t user_defined_length);
void change_display_value_to(char *data,unsigned char dot_status);

#endif