/* Question: why UART's error flag(overrun, parity) is set every time */
/* WARN: 
1. form app should start first, then the training then can continue.
2. 
3. !! first byte in encode data is maze.State now.
 */
/*
TODO: 
1. !!! encode and waitForConnection function may have infinite loop, hence a watchdog(use timestamp) may needed. (CAN CHECK IT WITH INTERR   UPT, LIKE WDT, IF VALUE GREATER THAN THRESHOLD, SET UART_STATE TO RECV_DONE)
2. a total error count should implement since sometimes a RST is enough to make the module back to normal.
3. X but should in IT) each send should expect an ACK during training.
4. (X) each send should clear maze.isDataChange.
5. we should implement a function to prevent esp8266's halt problem, like AT to know that if it's good.
6. !!receive during training should implement in IT mode since blocking mode will interfere Maze measurement
7. each state sohuld have longest live time, or we should consider as watchdog timeout.
*/


#include "esp8266.h"
#include "maze.h"
#include <string.h>


/* Global Variable ---------------*/
__IO uint8_t esp8266_sendBuffer[SEND_BUFFER_SIZE] = {0};
__IO uint8_t esp8266_recvBuffer[RECV_BUFFER_SIZE] = {0};
__IO uint8_t trainingDataBuffer[SEND_BUFFER_SIZE] = {0};
__IO int debug;
__IO extern  UART UART_esp8266;
__IO extern  Maze maze;
/* Extern function declaration ---------------*/

/* Function declaration ---------------*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) /* UART error(parity error, overrun...) callback */
{
    UART_esp8266.state = RECV_ERROR;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
    UART_esp8266.state = RECV_DONE;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* whole code existing here now can put into a if which has a state said that this is initialize state */
    UART_esp8266.state = SEND_DONE;
    if (UART_esp8266.isDataSendRD != YES)
	    memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE/* strlen((char*) &esp8266_sendBuffer) */);    /* argument pass to size is intend to improve the performance */
		
    //memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE/*strlen((char*) &esp8266_recvBuffer)*/);
	
    /*
		if (debug == 1)
			HAL_UART_Receive_IT(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, ECHO_TURN_OFF_EDGE);
		else if (debug == 2)
			HAL_UART_Receive_IT(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, TYPICAL_RECEIVE_SIZE);
    */
}

void wifi_sendCommand(uint8_t* command, uint8_t* specific)
{
    uint8_t index = 0, index_2 = 0;
    while (command[index] != NULL)
    {
        esp8266_sendBuffer[index++] = command[index]; /* armcc and gcc has different parser, hence if this line run on gcc ,the ++ srhould move to the end */
    }
    if (specific == NULL)
    {
        esp8266_sendBuffer[index++] = '\r';
        esp8266_sendBuffer[index++] = '\n';
        esp8266_sendBuffer[index]   = '\0';
    }
    else
    {
        esp8266_sendBuffer[index++] =  '=';
        while(specific[index_2] != NULL)
        {
            esp8266_sendBuffer[index++] =  specific[index_2++];
        }
        esp8266_sendBuffer[index++] = '\r';
        esp8266_sendBuffer[index++] = '\n';
        esp8266_sendBuffer[index]   = '\0';
    }
    HAL_UART_Transmit_IT(UART_esp8266.huart, (uint8_t*) &esp8266_sendBuffer, strlen((char*) &esp8266_sendBuffer));
    return;
}

void wifi_recvCheck(void)
{
    uint8_t index = 0;
    for (index = 0; index < RECV_BUFFER_SIZE - 1; index++) /* -1 here is to prevent the access of the address outside the buffer, and "OK" don't appears near the end of the buffer typically */
    {
        if (esp8266_recvBuffer[index] == 'O' && esp8266_recvBuffer[index + 1] == 'K')
        {
            UART_esp8266.wifiState = CHECK_PASS;
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
            return;
        }   
    }
    UART_esp8266.wifiState = CHECK_FAILED;
    memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
    return;
}

StatusTypeDef wifi_waitForResponse(void)
{
    while (UART_esp8266.state != SEND_DONE) {} /* block till serial data sent */
    if (UART_esp8266.wifiState != ATE0_DONE)
	    HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, TYPICAL_RECEIVE_SIZE, 2000);
    else
    {
        HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, ECHO_TURN_OFF_EDGE, 2000);
        UART_esp8266.wifiState = INIT; // to switch the receive size 
    }
    UART_esp8266.state = RECV_DONE;
    //while (UART_esp8266.state != RECV_DONE) {HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12); HAL_Delay(50);}
    //HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15, GPIO_PIN_SET);
    wifi_recvCheck();
    if (UART_esp8266.wifiState != CHECK_PASS) {UART_esp8266.wifiState = MODULE_ERROR; return FAILED;}
    HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_13);
    HAL_Delay(500); /* maybe can remove this since every time run this line is that 8266 done its transmission */
    return SUCCESSED;
}

void wifi_Init(void)
{
    wifi_sendCommand(AT_RST, NULL);
    HAL_Delay(2000);
    wifi_sendCommand(AT_NO_ECHO, NULL);
    
    UART_esp8266.wifiState = ATE0_DONE; 
    if (wifi_waitForResponse() == FAILED) return;
    wifi_sendCommand(AT_CWMODE, "2");
    if (wifi_waitForResponse() == FAILED) return;
    wifi_sendCommand(AT_CWSAP, "\"cLab\",\"00001111\",9,3,2,0");
    if (wifi_waitForResponse() == FAILED) return;
    wifi_sendCommand(AT_CIPMUX, "1");
    if (wifi_waitForResponse() == FAILED) return;
    wifi_sendCommand(AT_CIPCLOSE, "5");
    if (wifi_waitForResponse() == FAILED) return;
    wifi_sendCommand(AT_CWDHCP, "0,1");
    if (wifi_waitForResponse() == FAILED) return;
    UART_esp8266.wifiState = RUNNING_UNCONNECT;
}
/* @brief Wait for device connection in blocking mode(500ms) */
void wifi_waitForConnect(void)
{
    uint8_t index;
    wifi_sendCommand(AT_CWLIF, NULL); /* send this command to aquire connected device's information */
    while (UART_esp8266.state != SEND_DONE) {}
    if (HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, DEVICE_CONNECTED_SIZE, 1000) == HAL_TIMEOUT)
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
        UART_esp8266.state = STANDBY;   
        return; /* no device connect */
    }    
    for (index = 0; index < 37; index++)    /* check if received data is ip address or not */
    {
        if (esp8266_recvBuffer[index] == '1' && esp8266_recvBuffer[index + 1] == '9')
        {
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* total error count can be implement here */
            UART_esp8266.state = STANDBY;
            break;
        }        
        if (index == 36)
        {
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* total error count can be implement here */
            UART_esp8266.state = STANDBY;
            return;
        }            
    }
    UART_esp8266.wifiState = RUNNING_CONNECTED;
    UART_esp8266.state = STANDBY;
    memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
    return;
    
}
void wifi_establishConnetion(void)
{
    /*
    first argument is connection id and the fourth arg is remote port, since we just reset the module, hence we can use 0.
    comes to remote port, since the computer is in LAN, hence we can use the specific port, or it may be error.anyway this 
    reduce the flexibility in some case. 
    */
    wifi_sendCommand(AT_CIPSTART, "0,\"UDP\",\"192.168.4.2\",60138,3232,2"); /* WARN: last arg may cause error or not, just take a note */
    while (UART_esp8266.state != SEND_DONE) {}
    if (HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, DEVICE_CONN_ESTABLISH_SIZE, 1000) == HAL_TIMEOUT)
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* connection didn't establish */
        UART_esp8266.state = STANDBY;
        return; 
    }
    if (esp8266_recvBuffer[DEVICE_CONN_ESTABLISH_SIZE - 4] != 'O' || esp8266_recvBuffer[DEVICE_CONN_ESTABLISH_SIZE - 3] != 'K')
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* total error count can be implement here */
        UART_esp8266.state = STANDBY;
        return;
    }
    UART_esp8266.state = STANDBY;
    UART_esp8266.wifiState = RUNNING_ESTABLISHED;
    memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
    
    return;
}
void wifi_sendRawData(uint8_t* data)
{
    uint8_t index = 0;
    while (data[index] != NULL)
    {
        esp8266_sendBuffer[index++] = data[index]; /* since this is raw data hence \r\n is not needed */
    }
    HAL_UART_Transmit_IT(UART_esp8266.huart, (uint8_t*) &esp8266_sendBuffer, strlen((char*) &esp8266_sendBuffer));
    return;
}
void wifi_knockWait(void)
{
    /* a wait time 3510 can guarantee receive the knock packet from PC since PC ack wait time is 3 sec, and a timer interval 
    is 0.5 sec, hence its guaranteed.
    */
    if (HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, KNOCK_RECEIVE_SIZE, KNOCK_MAXIMUM_WAIT_TIME) == HAL_TIMEOUT)
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* connection didn't establish */
        UART_esp8266.state = STANDBY;
        return; 
    }
    if (esp8266_recvBuffer[KNOCK_RECEIVE_SIZE - 6] != 'r' || esp8266_recvBuffer[KNOCK_RECEIVE_SIZE - 5] != 'd')
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* total error count can be implement here */
        UART_esp8266.state = STANDBY;
        return;
    }
    //memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
    if (wifi_parseFoodArm() == FAILED)
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
        UART_esp8266.state = STANDBY;
        return;
    }
    wifi_sendCommand(AT_CIPSEND, "0,3"); /* send 3 bytes to PC which connection id is 0, since only one client, hence it's safe with connection id 0 */
    while (UART_esp8266.state != SEND_DONE) {}
    if (HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, SEND_PACKET_WITH_MODULE, SEND_DATA_COMMAND_WAIT) == HAL_TIMEOUT)
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* connection didn't establish */
        UART_esp8266.state = STANDBY;
        return; 
    }
    if (esp8266_recvBuffer[SEND_PACKET_WITH_MODULE - 6] != 'O' || esp8266_recvBuffer[SEND_PACKET_WITH_MODULE - 5] != 'K')
    {
        memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* total error count can be implement here */
        UART_esp8266.state = STANDBY;
        return;
    }
    UART_esp8266.state = STANDBY;     /* prevent mis-determine */
    wifi_sendRawData(MODULE_ACK);    /* this is raw data packet hence we encode it directly */
    while (UART_esp8266.state != SEND_DONE) {}
    UART_esp8266.state = STANDBY;
    UART_esp8266.wifiState = RUNNING_START_BUTTON_WAITING;
        /* debug */      HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12, GPIO_PIN_SET);
    return;
}
void wifi_sendData(void)
{
    if (HAL_GetTick() > (UART_esp8266.sendTick + PACKET_SEND_INTERVAL))     /* non-blocking wait, send packet minimum interval is 400 ms */
    {        
        UART_esp8266.sendTick = HAL_GetTick(); /* refresh the timestamp */
        wifi_sendCommand(AT_CIPSEND, "0,64"); /* send 3 bytes to PC which connection id is 0, since only one client, hence it's safe with connection id 0 */
        while (UART_esp8266.state != SEND_DONE) {}
        /*
        if (HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, SEND_PACKET_WITH_MODULE, SEND_DATA_COMMAND_WAIT - 100) == HAL_TIMEOUT)  -100 here is that in this is a time critical state(maze data should update ASAP) 
        {
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
            memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE);
            UART_esp8266.state = STANDBY;
            return; 
        }
        
        if (esp8266_recvBuffer[SEND_PACKET_WITH_MODULE - 6] != 'O' || esp8266_recvBuffer[SEND_PACKET_WITH_MODULE - 5] != 'K')
        {
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   
            memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE);
            UART_esp8266.state = STANDBY;
            return;
        }
        */
       
        HAL_Delay(50); /* CAN BE LOWER (UNTEST) block to wait "ok" to send training, note that this is really a bad implement since 
        the maze measurement got affect by blocked, but if change to non-blocking sometimes corrupt the UART, I suspect that its caused by I use recv IT and trans IT too close maybe*/
        HAL_UART_Transmit_IT(UART_esp8266.huart, (uint8_t*) &trainingDataBuffer, SEND_BUFFER_SIZE); 
        while (UART_esp8266.state != SEND_DONE) {}
        /* expected data: ACK */
        if (HAL_UART_Receive(UART_esp8266.huart, (uint8_t*) &esp8266_recvBuffer, SPECIAL_PACKET_SIZE, PACKET_CRITICAL_WAIT) == HAL_TIMEOUT) /* -100 here is that in this is a time critical state(maze data should update ASAP) */
        {
            memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE);
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);
            UART_esp8266.state = STANDBY;
            return; 
        }
        if (esp8266_recvBuffer[SPECIAL_PACKET_SIZE - 2] != 'C' || esp8266_recvBuffer[SPECIAL_PACKET_SIZE - 1] != 'K')
        {
            memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE);
            memset(&esp8266_recvBuffer, 0, RECV_BUFFER_SIZE);   /* total error count can be implement here */
            UART_esp8266.state = STANDBY;
            return;
        }
        memset(&esp8266_sendBuffer, 0, SEND_BUFFER_SIZE);
        memset(&trainingDataBuffer, 0, SEND_BUFFER_SIZE);
        memset(&esp8266_recvBuffer, 0, MODULE_RECEIVED_DATA); /* maybe can improve some performance */
        UART_esp8266.state = STANDBY;
        UART_esp8266.isDataSendRD = NO;        
        return;
    }
    return;
}
void wifi_trainingDataEncode(void)
{
    __IO uint8_t* __IO bufferPtr = (uint8_t*) &trainingDataBuffer;
    __IO uint8_t* __IO seqPtr    = (uint8_t*) &maze.EnterSeq;
    uint8_t index = 0;
    
    *bufferPtr++ = maze.State;
    for (index = 0; index < 8; index++)
    {
        *bufferPtr++ = maze.Arm[index].LongTerm_err;
        *bufferPtr++ = maze.Arm[index].ShortTerm_err;
    }
    *bufferPtr++ = maze.FoodCount;
    while (*seqPtr != NULL)
    {
        *bufferPtr++ = *seqPtr; /* TODO: i have no time to guarantee that how ++ is implemented here, hence I move it to next line */
        seqPtr++;
    }
    maze.isDataChange = NO;
    UART_esp8266.isDataSendRD = YES;
    return;
}

StatusTypeDef wifi_parseFoodArm(void)
{
    uint8_t index;
    uint8_t armBuffer;
    for (index = (KNOCK_RECEIVE_SIZE - 4); index < (KNOCK_RECEIVE_SIZE); index++)
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
    UART_esp8266.state = STANDBY;
    return SUCCESSED;
}
void wifi_startButtonWait(void)
{
    if (!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4))
        UART_esp8266.wifiState = RUNNING_TRAINING;

    return;
}
void wifi_process(void)
{
    switch (UART_esp8266.wifiState)
    {
        case DEBUG:
            
            break;
        case INIT:
            wifi_Init();    
            break;
        case MODULE_ERROR:
            if (UART_esp8266.errorCount++ > 2)
            {
                UART_esp8266.errorCount = 0;
                UART_esp8266.wifiState = INIT; /* fatal error occured, reset the module */
            }
            /* should go back to last state */
            break;
        case RUNNING_UNCONNECT:
            wifi_waitForConnect();
            break;
        case RUNNING_CONNECTED:
            wifi_establishConnetion();
            break;
        case RUNNING_ESTABLISHED:
            wifi_knockWait();
            break;
        case RUNNING_START_BUTTON_WAITING:
            wifi_startButtonWait();
            break;
        case RUNNING_TRAINING:
            /*
            if (maze.State == RAT_NOT_ENTERED)  we can say wait for rat, I think this could be TODO
            {
                
            }
            */            
            if (maze.isDataChange)
            {                
                wifi_trainingDataEncode();           
            }
            if (UART_esp8266.isDataSendRD)
            {
                wifi_sendData();
            }                        
            break;
        case RUNNING_TRAINING_DONE:
            /* send a finish packet */
            break;
        



    }
}


/************************ flawless0714 *****END OF FILE****/

