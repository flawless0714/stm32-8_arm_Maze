#ifndef __MAZE_H
#define __MAZE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdbool.h>

/* Function declaration-------------------------------------------------------*/
void ADC_Calibrator(void); /* once the ADC sample times > 1000000 times, it may start have deviation */
void Get_IR_Dist(void);

/** 
  * @brief Maze_arm_food Enum which stands for state of food in the arm  
  */
typedef enum
{
    WAIT_FOR_RAT = 1,
    RAT_NOT_ENTERED,
    RAT_ENTERED,
    TRANING_END
}Maze_state; 
/** 
  * @brief Maze_arm_food Enum which stands for state of food in the arm  
  */ 
typedef enum
{
    no_food,
    food_exist,
    food_ate
}Maze_arm_food;

/** 
  * @brief Maze_arm Structure definition of arm in Maze  
  */ 
typedef struct
{
    uint32_t noiseStartTick;
    uint16_t Dist;
    uint8_t LongTerm_err;
    uint8_t ShortTerm_err;
    bool    isLastEnter;
    bool    Enable;            
    Maze_arm_food Food;
}Maze_arm;

/** 
  * @brief Variability The variability found using tick1 and tick2 with value Dist 
  */ 
typedef struct
{
    uint32_t startTick;
    uint32_t endTick;
    uint32_t delta;
}ADC_Variability;

typedef struct
{
    Maze_arm Arm[8];
    Maze_state State;
}Maze;


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_MAZE_H */

/************************ flawless0714 *****END OF FILE****/
