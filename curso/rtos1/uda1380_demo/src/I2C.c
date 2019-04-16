#include "I2C.h"
#include "board.h"
#include "sapi.h"

void i2c_init(void)
{

	//PINSEL_CFG_Type PinCfg;
	/*
	 * Init I2C pin connect
	 */
	/*
	PinCfg.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Funcnum = 3;

	PinCfg.Portnum = 0;	
	PinCfg.Pinnum = 0;	//P0.10 SDA2
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;	//P0.11 SCL2
	PINSEL_ConfigPin(&PinCfg);	*/

	/* I2C block ------------------------------------------------------------------- */
	// Initialize I2C peripheral
	/*I2C_Init(LPC_I2C1, 30000);*/

	/* Enable I2C2 operation */
	/*I2C_Cmd(LPC_I2C1, ENABLE);*/
	//######  Codido nuevo #######
	//LPC Open 3.02 configuration
	/*Board_I2C_Init(UDA1380_I2C_BUS);
	Chip_I2C_Init(UDA1380_I2C_BUS);
	Chip_I2C_SetClockRate(UDA1380_I2C_BUS, 30000);
	Chip_I2C_SetMasterEventHandler(UDA1380_I2C_BUS, Chip_I2C_EventHandlerPolling);*/

	// sapi configuration
	i2cInit(I2C0, 30000);
}





