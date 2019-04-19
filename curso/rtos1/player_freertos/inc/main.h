#ifndef _PLAYER_RTOS_H_
#define _PLAYER_RTOS_H_

/*==================[inclusions]=============================================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/


int main(void);

// FreeRTOS tasks
void mySetupTask(void * pvParameters);
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _PLAYER_RTOS_H_ */
