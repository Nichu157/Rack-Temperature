// Functions of 7 segment display
#include "display.h"

/****************************************************************************************/
/*	Values of numeric and some symbols to display on 7 segment led.......		*/
/*											*/
/*	Dot always off...								*/
/****************************************************************************************/
/*

0	----> 0B 1100 0000	----> 0xC0
1	----> 0B 1111 1001	----> 0xF9
2	----> 0B 1010 0100	----> 0xA4
3	----> 0B 1011 0000	----> 0xB0
4	----> 0B 1001 1001	----> 0x99
5	----> 0B 1001 0010	----> 0x92
6	----> 0B 1000 0010	----> 0x82
7	----> 0B 1111 1000	----> 0xF8
8	----> 0B 1000 0000	----> 0x80
9	----> 0B 1001 0000	----> 0x90

n	----> 0B 1010 1011	----> 0xAB
o	----> 0B 1010 0011	----> 0xA3
C	----> 0B 1100 0110	----> 0xC6

F	----> 0B 1000 1110	----> 0x8E
A	----> 0B 1000 1000	----> 0x88
L	----> 0B 1100 0111	----> 0xC7

H	----> 0B 1000 1001	----> 0x89
E	----> 0B 1000 0110	----> 0x86

O	----> 0B 1100 0000	----> 0xC0
P	----> 0B 1000 1100	----> 0x8C

d	----> 0B 1010 0001	----> 0xA1
I	----> 0B 1111 1001	----> 0xF9

U	----> 0B 1100 0001	----> 0xC1
r	----> 0B 1010 1111	----> 0xAF
S	----> 0B 1001 0010	----> 0x92
t	----> 0B 1000 0111	----> 0x87
y	----> 0B 1001 0001	----> 0x91
*/

void change_display_value_to(char *data,unsigned char dot_status)
{
	G_display_data[0]		=	find_7_segment_value(*data++);
	G_display_data[1]		=	find_7_segment_value(*data++);
	G_display_data[2]		=	find_7_segment_value(*data);
	if(dot_status)
		G_display_data[1]	&=	0x7F;
}

/***********************************************************************************************************************
* Function Name: find_7_segment_value
* Description  : This function is used to find 7 segment value to display on 7 segment
* Arguments    : unsigned char
* Return Value : unsigned char
***********************************************************************************************************************/
unsigned char find_7_segment_value(unsigned char data)
{
	switch(data)
	{
		case '0':
		case 'O':
			return 0xC0;
		case '1':
		case 'I':
		case 'l':
			return 0xF9;
		case '2':
			return 0xA4;
		case '3':
			return 0xB0;
		case '4':
			return 0x99;
		case '5':
			return 0x92;
		case '6':
			return 0x82;
		case '7':
			return 0xF8;
		case '8':
			return 0x80;
		case '9':
			return 0x90;
		case 'H':
			return 0x89;
		case 'E':
			return 0x86;
		case 'L':
			return 0xC7;
		case 'F':
			return 0x8E;
		case 'A':
			return 0x88;
		case 'C':
			return 0xC6;
		case 'o':
			return 0xA3;
		case 'n':
			return 0xAB;
		case 'P':
		case 'p':
			return 0x8C;
		case 'd':
			return 0xA1;
		case 'U':
			return 0xC1;
		case 'r':
			return 0xAF;
		case 'S':
			return 0x92;
		case 't':
			return 0x87;
		case 'y':
			return 0x91;
		case ' ':
			return 0xFF;
		case '-':
			return 0xBF;
		default:
			return 0xFF;
	}
}


// Unsigned integer to string conversion
// No end char at the end of the string
unsigned char uint_to_string(unsigned char *add,unsigned int data,uint8_t user_defined_length)
{
	uint8_t i;
	#if 0
	unsigned char length;
	unsigned int value;
	
	value		=	data;
	length 		= 	0;
	do
	{
		value	/=	10;
		length++;
	}while(value != 0);
	value	=	length;
	#endif
	for(i = user_defined_length; i > 0;)
	{
		*(add + (--i))	= 	(data % 10) + '0';
		data		/=	10;
	}
	if(*add == '0')
		*add	=	' ';
	return user_defined_length;
}