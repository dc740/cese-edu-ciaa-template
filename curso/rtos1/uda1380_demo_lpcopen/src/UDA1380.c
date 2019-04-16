#include "UDA1380.h"
#include "board.h"
#include "stdint.h"

/* 
 *������С����
 */
//#define	LOUD				      0x0
//#define	SOFT				      0x0f
//#define DUMB				      0x3f


//#define DEVICE_ADDR               (0x30>>1)

#define UDA1380_REG_EVALCLK	      0x00
#define UDA1380_REG_I2S		      0x01
#define UDA1380_REG_PWRCTRL	      0x02
#define UDA1380_REG_ANAMIX	      0x03
#define UDA1380_REG_HEADAMP	      0x04
#define UDA1380_REG_MSTRVOL	      0x10
#define UDA1380_REG_MIXVOL	      0x11
#define UDA1380_REG_MODEBBT	      0x12
#define UDA1380_REG_MSTRMUTE      0x13
#define UDA1380_REG_MIXSDO	      0x14
#define UDA1380_REG_DECVOL	      0x20
#define UDA1380_REG_PGA		      0x21
#define UDA1380_REG_ADC		      0x22
#define UDA1380_REG_AGC		      0x23

#define UDA1380_REG_L3		      0x7f
#define UDA1380_REG_HEADPHONE     0x18
#define UDA1380_REG_DEC		      0x28


//I2C_M_SETUP_Type transferMCfg; Not used

uint8_t UDA1380InitData[][3] =
{
	/*
	 *Enable all power for now
	 */
	{UDA1380_REG_PWRCTRL,     0xA5, 0xDF},
	
	/*
	 *CODEC ADC and DAC clock from WSPLL, all clocks enabled
	 */ 
	{UDA1380_REG_EVALCLK,     0x0F, 0x39},
	
	/*
	 *I2S bus data I/O formats, use digital mixer for output
	 *BCKO is slave
	 */
	{UDA1380_REG_I2S,         0x00, 0x00},

	/*
	 *Full mixer analog input gain
	 */
	{UDA1380_REG_ANAMIX,      0x00, 0x00},
	
	/* 
	 *Enable headphone short circuit protection
	 */
	{UDA1380_REG_HEADAMP,     0x02, 0x02},
	
	/*
	 *Full master volume
	 */
	{UDA1380_REG_MSTRVOL,     0x55, 0x55},
	
	/*
	 *Enable full mixer volume on both channels
	 */
	{UDA1380_REG_MIXVOL,      0x00, 0x00},
	
	/*
	 *Bass and treble boost set to flat
	 */
	{UDA1380_REG_MODEBBT,     0x55, 0x15},
	
	/*
	 *Disable mute and de-emphasis
	 */
	{UDA1380_REG_MSTRMUTE,    0x00, 0x00},
	
	/*
	 *Mixer off, other settings off
	 */
	{UDA1380_REG_MIXSDO,      0x00, 0x00},
	
	/*
	 *ADC decimator volume to max
	 */
	{UDA1380_REG_DECVOL,      0x00, 0x00},
	
	/*
	 *No PGA mute, full gain
	 */ 
	{UDA1380_REG_PGA,         0x00, 0x00},
	
	/*
	 *Select line in and MIC, max MIC gain
	 */
	{UDA1380_REG_ADC,         0x0F, 0x02},
	
	/*
	 *AGC
	 */
	{UDA1380_REG_AGC,         0x00, 0x00},
	
	/*
	 *Disable clocks to save power
     *{UDA1380_REG_EVALCLK,     0x00, 0x32},
     *disable power to input to save power
     *{UDA1380_REG_PWRCTRL,     0xA5, 0xC0},
	 */
	 
    /*
     *End of list
	 */
	{0xFF,                    0xFF, 0xFF}
};


/*********************************************************************************************************
**�������ƣ�UDA1380Init																												
**�������ܣ���ʼ�����ݼ�������ע��				
**��ڲ���: ��																
**���ڲ���: ��	
**����˵����															
*********************************************************************************************************/
int32_t  UDA1380Init(void)
{  
	uint8_t dev_addr = 0; 
    uint8_t i = 0;
    int32_t errors = 0;
	/* 
	 *����P3.19~P3.21�ܽ�ΪL3�ӿڣ������Щ���ű�����ΪGPIO 
	 */
	//dev_addr = DEVICE_ADDR;

    while (1) 
	{
    
        if (UDA1380InitData[i][0] == 0xff) 
		{
            break;
        } 
		
		/* Start I2C slave device first */
        /*
        		transferMCfg.sl_addr7bit = dev_addr;
        		transferMCfg.tx_data = UDA1380InitData[i];
        		transferMCfg.tx_length = sizeof(UDA1380InitData[i]);
        		transferMCfg.retransmissions_max = 3;
        		if(I2C_MasterTransferData(UDA1380_I2C_BUS, &transferMCfg, I2C_TRANSFER_POLLING)==SUCCESS)
        		{*/

        //########### new code ####################
        //lpc open 3.02
        if (Chip_I2C_MasterSend(UDA1380_I2C_BUS, I2CDEV_UDA1380_ADDR, UDA1380InitData[i], sizeof(UDA1380InitData[i])) == sizeof(UDA1380InitData[i])) {
	     	i++;
			printf("Sends OK\r\n");
		}
		else
		{
			printf("Sends out the failure\r\n");
			errors++;
		}   
    }   
    return errors;
}

												                         
/*********************************************************************************************************
**End Of File
********************************************************************************************************/
