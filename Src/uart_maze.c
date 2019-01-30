/* 
    1. Due to unknown error of PC-end reception(dynamic extend recv array), we changed all data size send from mcu from 3 or specified size to SEND_BUFFER_SIZE, whereas 64 bytes.
*/
#include "uart_maze.h"

__IO uartTrans mazeTrans;
__IO extern UART UART_esp8266;
__IO extern Maze maze;
__IO uint32_t sendIntervalTick;
extern UART_HandleTypeDef huart3;
extern __IO uint8_t esp8266_sendBuffer[SEND_BUFFER_SIZE];
extern __IO uint8_t esp8266_recvBuffer[RECV_BUFFER_SIZE];



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
    mazeTrans.Tstate = RX_DONE;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_Receive_IT(huart, (uint8_t*) &esp8266_recvBuffer, UART_ACK_SIZE);
    UART_esp8266.sendTick = HAL_GetTick();
    mazeTrans.Tstate = DATA_SEND_DONE;
    sendIntervalTick = HAL_GetTick();
    /*
    switch (mazeTrans.Tstate)
    {
        case WAIT_KNOCK_DOOR:
            mazeTrans.Tstate = TRAINING_RUNNING;
            mazeTrans.state = RUNNING;
            break;
    }
    */
}

void uartTransmission(void)
{
    switch (mazeTrans.Tstate)
    {
        case WAIT_KNOCK_DOOR:
            /* wait RX callback inform RDY received */
            break;
        case TRAINING_RUNNING:
            HAL_UART_Receive_IT(&huart3, (uint8_t*) &esp8266_recvBuffer, UART_END_DATA_SIZE);
            if ((maze.isDataChange) && ((sendIntervalTick + 200) <= HAL_GetTick()))
            {
                HAL_UART_AbortReceive_IT(&huart3);
                memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE); /* with 0xff encode is that PC-end uses 0xff as data parse terminator*/
                uartTrainingDataEncode();                
                mazeTrans.Tstate = DATA_SEND_ONGOING;
                HAL_UART_Transmit_IT(&huart3, (uint8_t*)&esp8266_sendBuffer, SEND_BUFFER_SIZE);
                /* Rx done in Tx callback */
            }
            break;
        case DATA_SEND_DONE: /* here should send training data only */
            if ((UART_esp8266.sendTick + UART_TRAINING_DATA_WAIT_TIME) <= HAL_GetTick())
            {
                HAL_UART_AbortReceive_IT(&huart3);
                mazeTrans.Tstate = TRAINING_RUNNING;
            }
            /* training data sent done here, note that if Tx call back can't call HAL_UART_Receive_IT(), then a solution is add new state to mazeTrans to indicate state of transmission(done or ongoing)*/
            break;
        case DATA_SEND_ONGOING:
            
            break;
        /*
        case IDLE:

            break;
        case TX_BUSY:
            MAY NEED HAL_UART_AbortReceive_IT(&huart3) WARN: MAYBE RX TX NON-BLOCKING CANT WORK TOGETHER, THEREFORE OUR MCU GOT HALT AT ESP8266 PERIOD 
            break;
        case RX_BUSY:

            break;
        */
        case RX_DONE:
            switch (mazeTrans.state)
            {
                case WAIT_KNOCK:
                    if ((esp8266_recvBuffer[UART_KNOCK_DOOR_SIZE - 3] == 'R') && (esp8266_recvBuffer[UART_KNOCK_DOOR_SIZE - 2] == 'D') && (esp8266_recvBuffer[UART_KNOCK_DOOR_SIZE - 1] == 'Y'))
                    {                                                
                        esp8266_sendBuffer[0] = 'A';
                        esp8266_sendBuffer[1] = 'C';
                        esp8266_sendBuffer[2] = 'K';
                        HAL_UART_Transmit(&huart3, (uint8_t*)&esp8266_sendBuffer, SEND_BUFFER_SIZE, UART_TX_WAIT_TIME);  /* same size of knock back */

                        maze.State = RAT_NOT_ENTERED;                        
                        uartParseFoodArm();
                        maze.isDataChange = YES;

                        mazeTrans.state = RUNNING;
                        mazeTrans.Tstate = TRAINING_RUNNING;
                        UART_esp8266.sendTick = HAL_GetTick();
                        while ((UART_esp8266.sendTick + UART_KNOCK_DOOR_WAIT_PC) >= HAL_GetTick()); /* delay here since data is being to sent immediately */
                    }
                    else
                    {
                        mazeTrans.Tstate = WAIT_KNOCK_DOOR;
                        HAL_UART_Receive_IT(&huart3, (uint8_t*) &esp8266_recvBuffer, UART_KNOCK_DOOR_SIZE);
                    }
					memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
                    break;
                case RUNNING:
                    if ((esp8266_recvBuffer[0] == 'E') && (esp8266_recvBuffer[1] == 'N') && (esp8266_recvBuffer[2] == 'D')) /*  this makes training can be done anytime */
                    {												                        
                        Maze_Init();
                        esp8266_sendBuffer[0] = 'A';
                        esp8266_sendBuffer[1] = 'C';
                        esp8266_sendBuffer[2] = 'K';                        
                        HAL_UART_Transmit(&huart3, (uint8_t*)&esp8266_sendBuffer, SEND_BUFFER_SIZE, UART_TX_WAIT_TIME); /* this shouldn't timeout, but a check mechanism should be implemented (if HAL busy --> resend) */
						UART_esp8266.sendTick = HAL_GetTick();
                        while ((UART_esp8266.sendTick + UART_KNOCK_DOOR_WAIT_PC) >= HAL_GetTick()); /* delay here since data is being to sent immediately */
                        memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE);
                        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
                        //HAL_UART_Abort(&huart3);
                        //HAL_UART_Abort_IT(&huart3);
                        uartTransInit();
                        maze.isDataChange = NO;
                    }
                    else if ((esp8266_recvBuffer[0] == 'A') && (esp8266_recvBuffer[1] == 'C') && (esp8266_recvBuffer[2] == 'K'))
                    {
                        mazeTrans.Tstate = TRAINING_RUNNING;
                        maze.isDataChange = NO;                        
                    }
                    else
                    {
                        mazeTrans.Tstate = DATA_SEND_DONE;
                    }
					memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
                    break;
            }
            break;
    }
}

void uartParseFoodArm(void)
{
    uint8_t index;
    uint8_t armBuffer;
    for (index = 0; index < 4; index++)
    {        
        /*  if received data are string, use this to decode
        armBuffer = esp8266_recvBuffer[index] - '0';
        if (armBuffer > 7)  arm location here is represented as 0~7 
        {
            return FAILED;
        }
        */
        armBuffer = esp8266_recvBuffer[index];
        maze.Arm[armBuffer].Food = FOOD_EXIST;
    }
    memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
}

void uartTrainingDataEncode(void)
{
    __IO uint8_t* __IO bufferPtr = (uint8_t*) &esp8266_sendBuffer;
    __IO uint8_t* __IO seqPtr    = (uint8_t*) &maze.EnterSeq;
    uint8_t index;
    
    *bufferPtr++ = maze.State;
    for (index = 0; index < 8; index++)
    {
        *bufferPtr++ = maze.Arm[index].LongTerm_err;
        *bufferPtr++ = maze.Arm[index].ShortTerm_err;
    }
    *bufferPtr++ = maze.FoodCount;
    while (*seqPtr != NULL) /*TODO: if dereference of (buffer address) + 1 not equal to 0, then a violation access is commited */
    {
        *bufferPtr++ = *seqPtr;
        seqPtr++;
    }
    //UART_esp8266.isDataSendRD = YES;
}

void uartTransInit(void)
{
    mazeTrans.state = WAIT_KNOCK;
    mazeTrans.Tstate = WAIT_KNOCK_DOOR;
    sendIntervalTick = 0;
    HAL_UART_Receive_IT(&huart3, (uint8_t*) &esp8266_recvBuffer, UART_KNOCK_DOOR_SIZE);
}































/* flawless0714-------EOF-----*/
