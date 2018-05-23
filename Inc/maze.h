#ifndef __MAZE_H
#define __MAZE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdbool.h>

typedef enum
{
    no_food,
    food_exist,
    food_ate
}Maze_arm_food;
/** 
  * @brief Maze_arm structure definition  
  */ 
typedef struct
{
    uint8_t Dist;
    uint8_t LongTerm_err;
    uint8_t ShortTerm_err;
    bool    isLastEnter;
    Maze_arm_food Food;
}Maze_arm;








#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_MAZE_H */

/************************ flawless0714 *****END OF FILE****/
