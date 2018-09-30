#ifndef __UART_MAZE_H
#define __UART_MAZE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdbool.h>
#include "esp8266.h"
#include "maze.h"

#define UART_KNOCK_DOOR_SIZE            7   /* with food location */
#define UART_ACK_SIZE                   3
#define UART_TX_WAIT_TIME               500
#define UART_END_DATA_SIZE              3
#define UART_TRAINING_DATA_WAIT_TIME    1500
#define UART_KNOCK_DOOR_WAIT_PC         300

void uartTransmission(void);
void uartTransInit(void);
void uartTrainingDataEncode(void);
void uartParseFoodArm(void);

typedef enum
{    
    WAIT_KNOCK,
    RUNNING
}taskState;

typedef enum
{
    WAIT_KNOCK_DOOR,
    TRAINING_RUNNING,
    DATA_SEND_DONE,
    DATA_SEND_ONGOING,
    RX_DONE,
}trainingState;

typedef struct
{
    taskState state;
    trainingState Tstate;
}uartTrans;
























#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_UART_MAZE_H */

/************************ flawless0714 *****END OF FILE****/

