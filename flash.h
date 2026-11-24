//-----------------------------------------------------------------------------------------------------
					// FLASH DATA LIBRARY
//-----------------------------------------------------------------------------------------------------

#ifndef _PFDL_H
#define _PFDL_H

#include "pfdl_types.h"
#include "pfdl.h"

#define FLASH_START_ADDRESS     (0xF1000U)                              /* Start address of data flash               */
#define TARGET_BLOCK            (0U)                                    /* Block number of target(0 ~ 3)             */
#define BLOCK_SIZE              (0x400U)                                /* Block size                                */
#define WRITE_SIZE              (1U)                                    /* Size of data to be written at once        */
#define MAX_VALUE               (0xFFU)                                 /* Maximum value of writing                  */
#define MAX_ADDRESS             ((TARGET_BLOCK + 1U) * BLOCK_SIZE - 1U) /* Maximum address of writing                */
#define PFDL_NG                 (1U)                                    /* Failure to Data Flash                     */
#define FDL_FRQ                 (32U)                                   /* Setting frequency (MHz)                   */
#define FDL_VOL                 (0x00U)                                 /* Voltage mode                              */

//	DATA FLASH
#define BLOCK_0_START   0x0000U
#define BLOCK_0_END	0x03FFU
#define BLOCK_1_START   0x0400U
#define BLOCK_1_END	0x07FFU
#define BLOCK_0_SELECT  0U
#define BLOCK_1_SELECT  1U

extern void FDL_status_chk(void);						//Function that used to check the status of FDL Handler
extern void FDL_Blank_Chk(unsigned int ,unsigned int);				//Function that used to execute FDL blank check command
extern void FDL_Erase(unsigned char);						//Function that used to execute FDL erase command
extern void FDL_Write(unsigned int ,unsigned char *,unsigned int);		//Function that used to execute FDL write command
extern void FDL_Verify(unsigned int dest,unsigned int size);			//Function that used to execute FDL verify command
extern void FDL_Read(unsigned int ,unsigned char *,unsigned int);		//Function that used to execute FDL read command
extern void FDL_init(void);

extern unsigned int FDL_VersionChk; 						//buffer used to store the start address where the FDL version details located.

void write_rom_code_to_flash(void);
void perform_flash_check(void);
void write_set_point_offset_to_flash(void);

#endif