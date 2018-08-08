#ifndef __MAZE_H
#define __MAZE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"
#include <stdbool.h>

/* Function declaration -------------------------------------------------------*/
void Maze_Struct_Init(void);
void Maze_Rat_Detect(void);
void Maze_Init(void);
void Maze_Reset_Each_Arm_Delta(void);
/** 
  * @brief State of maze.  
  */
typedef enum
{
  WAIT_FOR_RAT = 1,
  RAT_NOT_ENTERED,
  RAT_ENTERED,
  TRAINING_END
}Maze_state; 
/** 
  * @brief State of food in the arm.
  */ 
typedef enum
{
  NO_FOOD,
  FOOD_EXIST,
  FOOD_ATE
}Maze_arm_food;

typedef enum
{
  NO = 0,
  YES 
}Maze_isData_Change;
/** 
  * @brief The variability found using tick1 and tick2 with converted ADC value.
  */ 
typedef struct
{
  uint32_t startTick; /* no use yet */
  uint32_t endTick; /* no use yet  */
  uint32_t delta;
}ADC_Variability;

/** 
  * @brief Structure definition of arm in Maze.
  */ 
typedef struct
{
  uint8_t LongTerm_err;
  uint8_t ShortTerm_err;         
  Maze_arm_food Food;
  ADC_Variability Variability;
}Maze_arm;
/** 
  * @brief Struct for 8_Arm_Maze
  */ 
typedef struct
{
  Maze_arm Arm[8];
  Maze_state State;
  uint8_t FoodCount;
  uint8_t LastEnteredArm;
  uint8_t EnterSeq[50]; /* enter counts should less than 50 times */
  uint8_t* EnterSeqPtr; /* maximux index is 49 , we should guarantee that enterSeq wont exceed 50 */
  Maze_isData_Change isDataChange; /* if data should update through wifi */
  uint32_t currentTick; /* cd between state change */
}Maze;


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_MAZE_H */

/************************ flawless0714 *****END OF FILE****/
