#include "maze.h"

extern ADC_HandleTypeDef hadc1;
__IO extern uint32_t ADC_BUF[8];
__IO extern uint32_t  SysTickITtimes;
//extern bool calibrating;
extern uint8_t str[5];
uint32_t tickEnd, L_SysTickITtimes;
//__IO bool ADC_Halt = false; /* make ADC more stable with no work while */
__IO uint8_t ADC_Maximum_capture_times = 9; 
__IO Maze_arm mazeArm[8] = {{0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food},
                       {0, 0, 0, 0, false, true, no_food}};


void ADC_Calibrator(void)
{
/*
  for (asLoop = 0; asLoop < 8; asLoop++) 
  if ((ADC_BUF[asLoop] > (mazeArm[asLoop].Dist + 130)) && mazeArm[asLoop].Enable) { mazeArm[asLoop].noiseStartTick = HAL_GetTick(); mazeArm[asLoop].Enable = false;}

  for (asLoop = 0; asLoop < 8; asLoop++)
  {
    if ( (HAL_GetTick() - 2) > mazeArm[asLoop].noiseStartTick)  CAUTION: after -2 is performed, the result should less than startTime + 10, as the next noise is coming. 
    {
      mazeArm[asLoop].Enable = true;
      mazeArm[asLoop].Dist = (uint16_t) ADC_BUF[asLoop];
    } 
  }
*/
  if (((ADC_BUF[0] > 570) && mazeArm[0].Enable) || ((mazeArm[0].Dist != 0) && ((mazeArm[0].Dist + 70) < (ADC_BUF[0]))))
  {
    //mazeArm[0].noiseStartTick = HAL_GetTick();
    mazeArm[0].Enable         = false; /* enable SysTickIT */
    L_SysTickITtimes = SysTickITtimes;
		HAL_ADC_Stop_DMA(&hadc1);
	
  }
  else
	return;
  /*
  if (ADC_BUF[0] < (mazeArm[0].Dist + 70) && mazeArm[0].Enable == false)
    tickEnd = HAL_GetTick();
  else 
	return;
  */
  while (!(SysTickITtimes == L_SysTickITtimes + 500)); /* block till 500us passed */
  /*
  SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk); /* disable SysTickIT 
  SysTick->LOAD  = (uint32_t)(167999UL); /* 168000 - 1, spec: cortexM4 programming p247 
  SysTick->VAL   = 0UL;
  SysTick->CTRL  = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; 
  */
  HAL_ADC_Start_DMA(&hadc1, ADC_BUF, 8);
  ADC_Maximum_capture_times = 0;
}
/**
  * @brief  
  * @retval None
  */
void Get_IR_Dist(void)
{
	str[0] = (uint8_t) (transferFunctionLUT5V[mazeArm[0].Dist/4]);
}
/*-------------------EOF------------------*/
