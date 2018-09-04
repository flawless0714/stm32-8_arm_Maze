/* TODO:
1. arm with food have not set, or it want to be const...not sure
 */

#include "maze.h"
#include "esp8266.h"
#include "uart_maze.h"

extern ADC_HandleTypeDef hadc1;
extern __IO uint32_t ADC_BUF[8];
extern uint8_t str[5];
__IO uint32_t Maze_LastEnterTick;
__IO Maze maze;
__IO uint32_t debugMostValue = 0;
__IO extern UART UART_esp8266;
__IO extern uartTrans mazeTrans;
                 
void Maze_Init(void)
{
  uint8_t i;
  Maze_LastEnterTick = 0; /* tick used in rat detection */
  for (i = 0; i < 8; i++)
  {
    maze.Arm[i].LongTerm_err = 0;
    maze.Arm[i].ShortTerm_err = 0;
    maze.Arm[i].Food = NO_FOOD; /* WARN: arm with food has enum FOOD_EXIST */
    maze.Arm[i].Variability.startTick = 0;
    maze.Arm[i].Variability.endTick = 0;
    maze.Arm[i].Variability.delta = 0;
  }
  maze.State = WAIT_FOR_RAT;
  /*
  for (i = 0; i < 8; i += 2)
  {
    maze.Arm[i].Food = FOOD_EXIST;
  }
  */
  maze.FoodCount = 0;
  maze.LastEnteredArm = 10; /* sthould not be value beteween 0~7 since they are arm symbol */
  for (i = 0; i < 50; i++)
  {
    maze.EnterSeq[i] = 0;
  }
  maze.isDataChange = NO;
  maze.EnterSeqPtr = (uint8_t*) &maze.EnterSeq;
  maze.currentTick = 0;
}

void Maze_Rat_Detect(void)
{// TODO bring this back on release version
  if (mazeTrans.Tstate == WAIT_KNOCK_DOOR) /* training have not ready to start */
  {
    return;
  }
  uint8_t i;
  if ((Maze_LastEnterTick + 500) <= HAL_GetTick())
  {
    Maze_LastEnterTick = HAL_GetTick(); /* refresh the tick */
    for (i = 0; i < 8; i++)
      maze.Arm[i].Variability.delta = 0;

  }
  switch (maze.State)
  {
    case WAIT_FOR_RAT: /* the state is being changed once position at center  is triggered */
    {
      for (i = 0; i < 8; i++)
      {
        switch (i) /* use another switch case to dealing with different characteristic of IRs, TODO: the value can be macroed, especially magic number 10000 */
        {
          case 0:
          {
            if (ADC_BUF[0] > 35 && ADC_BUF[0] < 43) /*&& (ADC_BUF[0] < 425)*///((ADC_BUF[i] > 121) && (ADC_BUF[i] < 125))
            {
							maze.Arm[0].Variability.delta++;
						}  
						if (debugMostValue < maze.Arm[i].Variability.delta)
							debugMostValue = maze.Arm[i].Variability.delta;
            if (maze.Arm[0].Variability.delta >= 55000) /* valid: 33397@200ms noise:s 6479@200ms */
						{
						  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
							maze.State = RAT_NOT_ENTERED;
						}
            break;
          }
          case 1:
          {
            if (ADC_BUF[i] > 32 && ADC_BUF[i] < 40)
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 55000)
            {
						  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
							maze.State = RAT_NOT_ENTERED;
						}
            break;
          }
          case 2:
          {
            if ((ADC_BUF[i] > 35) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 55000)
            {
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 3:
          {
            if ((ADC_BUF[i] > 28) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 87500)
            {
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_NOT_ENTERED;
            }   
            break;
          }
          case 4:
          {
            if ((ADC_BUF[i] > 28) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
						//if (debugMostValue < maze.Arm[i].Variability.delta)
						//		debugMostValue = maze.Arm[i].Variability.delta;
            if (maze.Arm[i].Variability.delta >= 55000)
            {
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_NOT_ENTERED;
            }   
            break;
          }
          case 5:
          {
            if ((ADC_BUF[i] > 34) && (ADC_BUF[i] < 42))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 55000)
            {
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_NOT_ENTERED;
            }   
            break;
          }
          case 6:
          {
            if ((ADC_BUF[i] > 28) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 55000)
            {
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_NOT_ENTERED;
            }   
            break;
          }
          case 7:
          {
            if ((ADC_BUF[i] > 38) && (ADC_BUF[i] < 46))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 55000)
            {
              maze.isDataChange = YES;
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_NOT_ENTERED;
            }   
            break;
          }
        }
      }
      break;
    }
    case RAT_NOT_ENTERED: /* the state is being changed once position Dc is triggered */
    {
      if ((maze.currentTick + 1450) >= HAL_GetTick()) /* cd between state change */
      {
        break;
      }
       for (i = 0; i < 8; i++)
      {
        switch (i) /* use another switch case to dealing with different characteristic of IRs, TODO: the value can be macroed */
        {
          case 0:
          {
            if (ADC_BUF[i] > 44 && ADC_BUF[i] < 100)
              maze.Arm[i].Variability.delta++;  
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;
              
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST) /* food counting */
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }
            }
            break;
          }
          case 1:
          {
            if (ADC_BUF[1] > 50 && ADC_BUF[1] < 100)
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;            
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }                
            }
            break;
          }
          case 2:
          {
            if ((ADC_BUF[i] > 52) && (ADC_BUF[i] < 98))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }                
            }
            break;
          }
          case 3:
          {
            if ((ADC_BUF[i] > 42) && (ADC_BUF[i] < 83)) /* was 79 0821 15:55 */
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 49750)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }
            }
            break;
          }
          case 4:
          {
            if ((ADC_BUF[i] > 50) && (ADC_BUF[i] < 100))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;             
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }                
            }
            break;
          }
          case 5:
          {
            if ((ADC_BUF[i] > 47) && (ADC_BUF[i] < 100))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))   /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1);  /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();  /* reset to prevent duplicated determination */
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;    
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }
            }
            break;
          }
          case 6:
          {
            if ((ADC_BUF[i] > 43) && (ADC_BUF[i] < 100))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;             
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }                
            }
            break;
          }
          case 7:
          {
            if ((ADC_BUF[i] > 57) && (ADC_BUF[i] < 90))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 48500)
            {
              if ((void*) maze.EnterSeqPtr != (void*) (&maze.EnterSeq + 50U))  /* prevent access of address out of array */
                *maze.EnterSeqPtr++ = (i + 1); /* arm 1~8 */
              Maze_Reset_Each_Arm_Delta();
              maze.State = RAT_ENTERED;
              if (maze.Arm[i].Variability.startTick == 0) /* isLastArmEntered ? */
              {
								maze.Arm[i].Variability.startTick++;
              }
							else
							{
								maze.Arm[i].ShortTerm_err++;
							}
							maze.isDataChange = YES;             
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == FOOD_EXIST)
              {
                maze.Arm[i].Food = FOOD_ATE;
                maze.FoodCount++;
                maze.isDataChange = YES;
              }
              else if(maze.Arm[i].Food == NO_FOOD && maze.Arm[i].LongTerm_err == 0)
              {
                maze.Arm[i].LongTerm_err++;
                maze.isDataChange = YES;
              }                
            }
            break;
          }
        }
      }
      return;
    }
    case RAT_ENTERED: /* RAT_NOT_ENTERED is triggered by Da */
    {
      if ((maze.currentTick + 950) >= HAL_GetTick()) /* cd between state change */
      {				
        break;
      }
			i = maze.LastEnteredArm;
      if (maze.FoodCount >= 4)
      {
        maze.State = TRAINING_END;
        maze.isDataChange = YES;
        return;
      }
        switch (maze.LastEnteredArm) /* use another switch case to dealing with different characteristic of IRs, TODO: the value can be macroed */
        {
          case 0:
          {
            if ((ADC_BUF[i] > 27) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 45000)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;              
            }              
            break;
          }
          case 1:
          {
            if ((ADC_BUF[i] > 30) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 43500)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 2:
          {
            if ((ADC_BUF[i] > 33) && (ADC_BUF[i] < 41))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 44000)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 3:
          {
            if ((ADC_BUF[i] > 26) && (ADC_BUF[i] < 34))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 45000)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 4:
          {
            if ((ADC_BUF[i] > 21) && (ADC_BUF[i] < 31))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 43000)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 5:
          {
            if ((ADC_BUF[i] > 28) && (ADC_BUF[i] < 36))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 44000)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 6:
          {
            if ((ADC_BUF[i] > 24) && (ADC_BUF[i] < 30))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 43500)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
          case 7:
          {
            if ((ADC_BUF[i] > 34) && (ADC_BUF[i] < 42))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 44000)
            {
              Maze_Reset_Each_Arm_Delta();
              maze.isDataChange = YES;
              maze.State = RAT_NOT_ENTERED;
            }              
            break;
          }
        }
      
      return;
    }
    case TRAINING_END:
    {
      return;
    }
  }
    //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);//maze.Arm[0].Variability.startTick = uwTick;
}
/*  @brief reset delta to get a cleared val at next state and update ticktime
 *
 *
 */
void Maze_Reset_Each_Arm_Delta(void)
{
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    maze.Arm[i].Variability.delta = 0;
  }
  maze.currentTick = HAL_GetTick();
}

/************************ flawless0714 *****END OF FILE****/
