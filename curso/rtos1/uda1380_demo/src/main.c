#include "board.h"
#include "sapi.h"     // <= sAPI header
#include "I2C.h"
#include "I2S.h"
#include "UDA1380.h"
#include "wavData.h"

#define	  SNDTHELAST		(I2SSndPt  > WAVFILESIZ - 4)
/* 
 *  �����ļ���С 
 */
#define	  WAVFILESIZ		4800000
#define   MusicStartPos		45

uint32_t	Snd_End=0;			                /* Ϊ1ʱ,����WAV�ļ��Ѿ��������*/
uint32_t 	I2SSndPt=0;			                /*  WAV��������ָ��   	 */
uint8_t test;

void  I2S_SndNchar( const unsigned char * datbuf, uint32_t* datbufpt_pt, uint32_t snd_N) ;
								 


/* Support required entry point for other toolchain */

int main (void)
{
//	uint32_t WavDat = 0;
    volatile uint32_t LastDatToSnd_N;
    volatile uint32_t Snd_End 	;			                    /* Ϊ1ʱ,����WAV�ļ��Ѿ��������*/

    //sapi things
    boardConfig();

	//delay_init(); //removed custom delay implementation
    printf("Welcome to use Open1768 development board of the I2S test:\r\n");
	i2c_init();
    printf("I2C initialization OK \r\n");
	i2s_init();

	delay(100);
    printf("I2S Initialization OK:\r\n");
	UDA1380Init();///??? esta funcion estaba comentada?por?!
	//delay_ms(1000);
    delay(1000);
	I2SSndPt = MusicStartPos;                                           /* ָ��WAV�ļ��������������ʼ��*/
	 	 
    while (Snd_End == 0)
    {   	
        if (SNDTHELAST) 
        {					                            /* �跢�͵������Ѿ�����4��������*/
                                                                        /* ʣ�µ������ֽ���ĿΪ1��2��3��*/
                                                                        /* ������ദ��                 */	
	    LastDatToSnd_N = WAVFILESIZ - I2SSndPt;
	    I2S_SndNchar(WaveData, &I2SSndPt, LastDatToSnd_N); 
	    Snd_End = 1;		
		} 
        else 
        {
	    	I2S_SndNchar(WaveData, &I2SSndPt, 4); 
		} 	  
    }		   
    printf("Finished playing.\r\n");
//	I2SOUT_STOP();                                                      /* ֹͣ�͸�λI2S���            */
	
	while  (1);
}


/*********************************************************************************************************
** Function name:       I2S_SndNchar
** Descriptions:        �����ݻ���datbuf��ĳ����ʼ����ָ����Ŀ���ֽڵ�I2S_FIFO,���Զ��������ݻ���ķ���ָ��
** input parameters:    uint8*  datbuf:        �跢�ͳ�ȥ�����ݻ����׵�ַ
**                      uint32* datbufpt_pt:   ��datbuf[*datbufpt_pt]��ʼ����4���ֽ�
**                      uint32  snd_N:         �跢�͵����ݸ���		
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
//void  I2S_SndNchar( uint8_t* datbuf, uint32_t* datbufpt_pt, uint32_t snd_N) 
void  I2S_SndNchar( const unsigned char * datbuf, uint32_t* datbufpt_pt, uint32_t snd_N) 
{
    uint32_t WavDat = 0;  	
    switch (snd_N){
  		
    case 4:
       WavDat	= (datbuf[*datbufpt_pt  + 2]  << 24)
		| (datbuf[*datbufpt_pt + 3]   << 16)
		| (datbuf[*datbufpt_pt]       << 8)
		| (datbuf[*datbufpt_pt + 1]);
       break;
    case 3:
	WavDat 	= (datbuf[*datbufpt_pt + 2]   << 24)
		| (datbuf[*datbufpt_pt]       << 8)
		| (datbuf[*datbufpt_pt + 1]);
	break;
    case 2:
	WavDat  = (datbuf[*datbufpt_pt]       << 8)
		| (datbuf[*datbufpt_pt + 1]);
	break;
    case 1:
	WavDat  = datbuf[*datbufpt_pt];
	break;
    default:
        break;
    }
    Chip_I2S_Send(UDA1380_I2S_BUS,WavDat);
    //Chip_I2S_Send(UDA1380_I2S_BUS,0xAAAAAAAA);  //DEBUG HELPER
    *datbufpt_pt   += snd_N;				
	//while (Chip_I2S_GetTxLevel(UDA1380_I2S_BUS) != 0); //original
    while (Chip_I2S_GetTxLevel(UDA1380_I2S_BUS) >= 4); //FIFO is actually 8x32bit
}

