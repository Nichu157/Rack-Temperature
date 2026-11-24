//-------------------------------------------------------------------------------------------------
				//	FLASH DATA LIBRARY.C
//-------------------------------------------------------------------------------------------------

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "string.h"
#include "flash.h"
#include "ds18b20.h"
#include "modbus_user.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

unsigned int FDL_VersionChk	   = 0; //buffer used to store the start address where the FDL version details located.

pfdl_descriptor_t FDL_SysInit	= {16,0};			//CPU clock 8MHz is used & Full speed mode selected as programming voltage mode
pfdl_status_t FDL_Status 	= PFDL_IDLE;			// Declaration of FDL_Status with initial value as PFDL_IDLE
pfdl_request_t FDL_Request 	= {0,0,0,PFDL_CMD_READ_BYTES};			// Declaration of FDL_Request with initial value

void FDL_status_chk(void);					//Function that used to check the status of FDL Handler
void FDL_Blank_Chk(unsigned int ,unsigned int );		//Function that used to execute FDL blank check command
void FDL_Erase(unsigned char );					//Function that used to execute FDL erase command
void FDL_Write(unsigned int ,unsigned char *,unsigned int );	//Function that used to execute FDL write command
void FDL_Verify(unsigned int dest,unsigned int size);		//Function that used to execute FDL verify command
void FDL_Read(unsigned int ,unsigned char *,unsigned int );	//Function that used to execute FDL read command
unsigned char cal_check_sum (unsigned char *start, signed char count);
void perform_uc_flash_write(void);
void perform_flash_check(void);
void copy_data_flash_to_ram(unsigned char ,unsigned int *,unsigned int ,unsigned int );
void FDL_init(void);



/* Start user code for adding. Do not edit comment generated here */
void FDL_init(void)
{
    FDL_Status=PFDL_Open(&FDL_SysInit);
    if (FDL_Status!= PFDL_OK)
    {
	    while(1); 								//FDL initialization failed, user may insert RESET function
    }
   // FDL_Write(dest,source,size);
}

void FDL_Blank_Chk(unsigned int block_add,unsigned int size)
{
	FDL_Request.index_u16		= block_add;				//Start Address of Data Flash (For R5F100LE, Data Flash range from (0xF1000 - 0xF1FFF)
	FDL_Request.bytecount_u16	= size;					//Counter for number of bytes to be check
	FDL_Request.command_enu		= PFDL_CMD_BLANKCHECK_BYTES;		//Assign the command to be execute by PFDL_Execute
	PFDL_Execute(&FDL_Request);						//Execute the Blank Check on data flash
	FDL_status_chk();							//Check status of FDL Handler
}

void FDL_status_chk(void)
{
	FDL_Status=PFDL_BUSY;
	while (FDL_Status==PFDL_BUSY)
	{
		FDL_Status=PFDL_Handler();
	}	
}

void FDL_Erase(unsigned char block_number)
{
	FDL_Request.index_u16	=	block_number;  				//Assign the block 0 of data flash to be erase Data Flash 
										//0: block 0 (0xF1000 to 0xF13FF) 
										//1: block 1 (0xF1400 to 0xF17FF)
										//2: block 2 (0xF1800 to 0xF1BFF)
										//3: block 3 (0xF1C00 to 0xF1FFF)
	FDL_Request.command_enu =	PFDL_CMD_ERASE_BLOCK;			//Assign the command to be execute by PFDL_Execute
	PFDL_Execute(&FDL_Request);						//Execute the Blank Check on data flash
	FDL_status_chk();							//Check status of FDL Handler
}

void FDL_Write(unsigned int dest,unsigned char *source,unsigned int size)
{
	FDL_Request.index_u16		= dest;  				//Start Address of Data Flash (For R5F100LE, Data Flash range from (0xF1000 - 0xF1FFF)
	FDL_Request.data_pu08		= source; 				//Assign the address of Write_DataBuffer as data to be stored in Data Flash
	FDL_Request.bytecount_u16	= size;					//Counter for number of bytes to be write
	FDL_Request.command_enu		= PFDL_CMD_WRITE_BYTES;			//Assign the command to be execute by PFDL_Execute
	PFDL_Execute(&FDL_Request);						//Execute the Blank Check on data flash
	FDL_status_chk();							//Check status of FDL Handler	
}

void FDL_Verify(unsigned int dest,unsigned int size)
{
	FDL_Request.index_u16		= dest;  				//Start Address of Data Flash (For R5F100LE, Data Flash range from (0xF1000 - 0xF1FFF)
	FDL_Request.bytecount_u16	= size;					//Counter for number of bytes to be verify
	FDL_Request.command_enu 	= PFDL_CMD_IVERIFY_BYTES;		//Assign the command to be execute by PFDL_Execute
	PFDL_Execute(&FDL_Request);						//Execute the data verify on data flash
	FDL_status_chk();
}

void FDL_Read(unsigned int source,unsigned char *dest,unsigned int size)
{
	FDL_Request.index_u16		= source;  				//Read from 0xF1000h; 
										//Start Address of Data Flash (For R5F100LE, Data Flash range from (0xF1000 - 0xF1FFF)
	FDL_Request.data_pu08		= dest; 				//Assign the address of Read_DataBuffer to stored contents of  dedicated Data Flash area
	FDL_Request.bytecount_u16	= size;					//Counter for number of bytes to be read
	FDL_Request.command_enu 	= PFDL_CMD_READ_BYTES;			//Assign the command to be execute by PFDL_Execute
	PFDL_Execute(&FDL_Request);						//Execute the read command on data flash
	FDL_status_chk();
}

void perform_flash_check(void)
{
    unsigned char buffer[31];
    
    //	FDL INITIALISATION
    FDL_init();
    FDL_Blank_Chk((unsigned int)BLOCK_0_START,BLOCK_SIZE);
    if (FDL_Status==PFDL_ERR_MARGIN)						//Execute Erase comamnd if Blank Check error( data present )
    {
    	FDL_Read((unsigned int)BLOCK_0_START,buffer,30U);
	memcpy((void *)&G_rom_code[0][0],buffer,10);
        memcpy((void *)&G_rom_code[1][0],&buffer[10],10);
        memcpy((void *)&G_rom_code[2][0],&buffer[20],10);
    }
    
    FDL_Blank_Chk((unsigned int)BLOCK_1_START,BLOCK_SIZE);
    if(FDL_Status==PFDL_ERR_MARGIN)
    {
	FDL_Read((unsigned int)BLOCK_1_START,buffer,6U);    
	G_set_point		= buffer[0];
	G_set_point		<<= 8;
	G_set_point		|= buffer[1];
	G_ds18b20[0].offset	= buffer[2];
	G_ds18b20[1].offset	= buffer[3];
	G_ds18b20[2].offset	= buffer[4];
	g_device_id		= buffer[5];
	
	g_sp_offset_tx_buff[0]	= G_set_point;
        g_sp_offset_tx_buff[1]	= G_ds18b20[0].offset;
        g_sp_offset_tx_buff[2]	= G_ds18b20[1].offset;
        g_sp_offset_tx_buff[3]	= G_ds18b20[2].offset;
    }
    PFDL_Close();
}

void write_rom_code_to_flash(void)
{
	unsigned char buffer[31];
    	memcpy(buffer,(void *)&G_rom_code[0][0],10);
	memcpy(&buffer[10],(void *)&G_rom_code[1][0],10);
	memcpy(&buffer[20],(void *)&G_rom_code[2][0],10);
    	//	FDL INITIALISATION
    	FDL_init();
	FDL_Erase(BLOCK_0_SELECT);
        FDL_Write((unsigned int)BLOCK_0_START,buffer,30U);
	PFDL_Close();
}

void write_set_point_offset_to_flash(void)
{
	unsigned char buffer[6];
    	buffer[0]		= G_set_point >> 8;
	buffer[1]		= G_set_point;
	buffer[2]		= G_ds18b20[0].offset;
	buffer[3]		= G_ds18b20[1].offset;
	buffer[4]		= G_ds18b20[2].offset;
	buffer[5]		= g_device_id;
	
	g_sp_offset_tx_buff[0]	= G_set_point;
        g_sp_offset_tx_buff[1]	= G_ds18b20[0].offset;
        g_sp_offset_tx_buff[2]	= G_ds18b20[1].offset;
        g_sp_offset_tx_buff[3]	= G_ds18b20[2].offset;
	
    	//	FDL INITIALISATION
    	FDL_init();
	FDL_Erase(BLOCK_1_SELECT);
        FDL_Write((unsigned int)BLOCK_1_START,buffer,6U);
	PFDL_Close();
}