#ifndef __ESP8266_H
#define __ESP8266_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "maze.h"
#include <string.h>
/* Macros ------------------------------------------------------------------*/
#define SSID "CCC_Lab\0"
#define PASSWORD "00001111\0"
#define MODULE_ACK  "ACK\0"
#define RECV_BUFFER_SIZE            64
#define SEND_BUFFER_SIZE            64
#define TYPICAL_RECEIVE_SIZE        6   /* for OK-like command's return */
#define ECHO_TURN_OFF_EDGE          13  /* at echo turn off edge, 8266 will have last echo so that the buffer should larger than typical */
#define DEVICE_CONNECTED_SIZE       37  /* only for one device */
#define DEVICE_CONN_ESTABLISH_SIZE  17  /* indicate that connection is established */
#define KNOCK_MAXIMUM_WAIT_TIME     3510
#define KNOCK_RECEIVE_SIZE          17  /* include food arm location */
#define SEND_PACKET_WITH_MODULE     8
#define PACKET_SEND_INTERVAL        400
#define SEND_DATA_COMMAND_WAIT      300
#define MODULE_RECEIVED_DATA        14
#define PACKET_CRITICAL_WAIT        50
#define SPECIAL_PACKET_SIZE         43  /* since we cant solve too high data rate, so this size received recv 64 bytes and msg from PC which is ACK  */
/* AT command ----------------------------------------------------------*/
#define AT_RST      "AT+RST\0"
#define AT          "AT\0"
#define AT_OK       "OK\0"
#define AT_CIPMUX   "AT+CIPMUX\0" /*  note that single connection maybe also work */
#define AT_NO_ECHO  "ATE0\0"
#define AT_CWMODE   "AT+CWMODE\0"
#define AT_CWSAP    "AT+CWSAP\0"
#define AT_CWDHCP   "AT+CWDHCP\0"
#define AT_CIPSTART "AT+CIPSTART\0"
#define AT_CWLIF    "AT+CWLIF\0"
#define AT_CIPSEND  "AT+CIPSEND\0"
#define AT_CIPCLOSE "AT+CIPCLOSE\0"

typedef enum
{
  FAILED = -1,
  SUCCESSED = 0 /* the name is used somewhere hence we plus "ED" */
}StatusTypeDef;
/* Function declaration-------------------------------------------------------*/
void wifi_Init(void);
void wifi_process(void);
void wifi_sendCommand(uint8_t* command, uint8_t* specific);
void wifi_recvCheck(void);
StatusTypeDef wifi_waitForResponse(void);
void wifi_waitForConnect(void);
void wifi_establishConnetion(void);
void wifi_knockWait(void);
void wifi_sendRawData(uint8_t* data);
void wifi_trainingDataEncode(void);
void wifi_sendData(void);
StatusTypeDef wifi_parseFoodArm(void);
void wifi_startButtonWait(void);

/* Enumerator -------------------------------------------------------*/
/** 
  * @brief State of AT command parser.  
  */
typedef enum
{
  WAIT_FOR_RESPONSE = 10,
  IDLE,
  PARSING_COMMAND
    
}AT_command_parser_state; 

typedef enum
{
  RECV_ERROR = -1,   /* UART parity error, overrun..., set from UART error callback */
  STANDBY,
  RECV_START,
  RECV_DONE,
  SEND_DONE,
  MALFORMED_RECEIVED
}UART_state; 

typedef enum
{
  INIT = 20,
  ATE0_DONE,
  CHECK_PASS,
  CHECK_FAILED,
  RUNNING_UNCONNECT,
  RUNNING_CONNECTED,
  RUNNING_ESTABLISHED,
  RUNNING_START_BUTTON_WAITING,
  RUNNING_TRAINING,
  RUNNING_TRAINING_DONE,
  DATA_SEND,
  DATA_RECEIVED,
  MODULE_ERROR,
  DEBUG
}ESP8266_state; 



typedef struct
{
  UART_state state;
  UART_HandleTypeDef *huart;
  ESP8266_state wifiState;
  uint8_t errorCount;
  uint32_t sendTick;
  Maze_isData_Change isDataSendRD;
}UART;



StatusTypeDef wifi_waitForResponse(void);




#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_ESP8266_H */

/************************ flawless0714 *****END OF FILE****/
