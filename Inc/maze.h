#ifndef __MAZE_H
#define __MAZE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdbool.h>

/* Function declaration-------------------------------------------------------*/
void Maze_Struct_Init(void);
void Maze_Rat_Detect(void);
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
    no_food,
    food_exist,
    food_ate
}Maze_arm_food;

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
    bool    isLastEnter;           
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
}Maze;


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_MAZE_H */

/************************ flawless0714 *****END OF FILE****/
