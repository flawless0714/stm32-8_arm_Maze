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
  uint32_t startTick; /* we use this to identify if the arm has entered since training start */
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

// arm number, for array indexing, we start from zero
#define ARM_1 0
#define ARM_2 1
#define ARM_3 2
#define ARM_4 3
#define ARM_5 4
#define ARM_6 5
#define ARM_7 6
#define ARM_8 7

// arm-specific IR dist. sensor characteristic (RAT_NOT_ENTERED)
#define ARM_1_N_L 44 // arm 1, **N**ot entered, **L**ower bound
#define ARM_1_N_U 100 // arm 1, **N**ot entered, **U**pper bound
#define ARM_2_N_L 53
#define ARM_2_N_U 101
#define ARM_3_N_L 52
#define ARM_3_N_U 98
#define ARM_4_N_L 47
#define ARM_4_N_U 99
#define ARM_5_N_L 50
#define ARM_5_N_U 101
#define ARM_6_N_L 47
#define ARM_6_N_U 100
#define ARM_7_N_L 43 
#define ARM_7_N_U 100 
#define ARM_8_N_L 55
#define ARM_8_N_U 90

// arm-specific IR dist. sensor characteristic (RAT_ENTERED)
#define ARM_1_E_L 23 // arm 1, **E**ntered, **L**ower bound
#define ARM_1_E_U 32 // arm 1, **E**ntered, **U**pper bound
#define ARM_2_E_L 22
#define ARM_2_E_U 32
#define ARM_3_E_L 32
#define ARM_3_E_U 40
#define ARM_4_E_L 24
#define ARM_4_E_U 32
#define ARM_5_E_L 17
#define ARM_5_E_U 28
#define ARM_6_E_L 22
#define ARM_6_E_U 30
#define ARM_7_E_L 22
#define ARM_7_E_U 28
#define ARM_8_E_L 20
#define ARM_8_E_U 39

// arm-specific trigger threshold (RAT_NOT_ENTERED)
#define ARM_1_N_THRES 49000
#define ARM_2_N_THRES 48500
#define ARM_3_N_THRES 48500
#define ARM_4_N_THRES 49750
#define ARM_5_N_THRES 48500
#define ARM_6_N_THRES 48500
#define ARM_7_N_THRES 48500
#define ARM_8_N_THRES 48500

// arm-specific trigger threshold (RAT_ENTERED)
#define ARM_1_E_THRES 46000
#define ARM_2_E_THRES 44500
#define ARM_3_E_THRES 45000
#define ARM_4_E_THRES 46000
#define ARM_5_E_THRES 44000
#define ARM_6_E_THRES 45000
#define ARM_7_E_THRES 44500
#define ARM_8_E_THRES 45000

#define is_IN_RANGE(arm_num, upper, lower) ((ADC_BUF[arm_num] > lower) && \
                   (ADC_BUF[arm_num] < upper))

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_MAZE_H */

/************************ flawless0714 *****END OF FILE****/
