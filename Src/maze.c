#include "maze.h"

extern ADC_HandleTypeDef hadc1;
extern __IO uint32_t ADC_BUF[8];
extern uint8_t str[5];
__IO uint32_t Maze_LastEnterTick;
__IO Maze maze = {{0},WAIT_FOR_RAT };
                 
void Maze_Init(void)
{
  uint8_t i;
  Maze_LastEnterTick = 0; /* tick used in rat detection */
  for (i = 0; i < 8; i++)
  {
    maze.Arm[i].LongTerm_err = 0;
    maze.Arm[i].ShortTerm_err = 0;
    maze.Arm[i].isLastEnter = false;
    maze.Arm[i].Food = no_food; /* WARN: arm with food its enum is food_exist */
    maze.Arm[i].Variability.startTick = 0;
    maze.Arm[i].Variability.endTick = 0;
    maze.Arm[i].Variability.delta = 0;
  }
  maze.State = WAIT_FOR_RAT;
  maze.FoodCount = 0;
  maze.LastEnteredArm = 10; /* sthould not be value beteween 0~7 since they are arm symbol */
}

void Maze_Rat_Detect(void)
{
  uint8_t i;
  if ((Maze_LastEnterTick + 100) <= HAL_GetTick())
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
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 1:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 2:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 3:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 4:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 5:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 6:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 7:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
        }
      }
      break;
    }
    case RAT_NOT_ENTERED: /* the state is being changed once position Dc is triggered */
    {
       for (i = 0; i < 8; i++)
      {
        switch (i) /* use another switch case to dealing with different characteristic of IRs, TODO: the value can be macroed */
        {
          case 0:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist) /* food counting */
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 1:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 2:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 3:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 4:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 5:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 6:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
          case 7:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
            {
              maze.State = RAT_ENTERED;
              if (i == maze.LastEnteredArm) /* isLastArmEntered ? */
                maze.Arm[i].ShortTerm_err++;
              maze.LastEnteredArm = i;
              if (maze.Arm[i].Food == food_exist)
              {
                maze.Arm[i].Food = food_ate;
                maze.FoodCount++;
              }
              else if(maze.Arm[i].Food == no_food && maze.Arm[i].LongTerm_err <= 4)
                maze.Arm[i].LongTerm_err++;
            }
            break;
          }
        }
      }
      return;
    }
    case RAT_ENTERED: /* RAT_NOT_ENTERED is triggered by Da */
    {
      if (maze.FoodCount >= 4)
      {
        maze.State = TRAINING_END;
        return;
      }
      for (i = 0; i < 8; i++)
      {
        switch (i) /* use another switch case to dealing with different characteristic of IRs, TODO: the value can be macroed */
        {
          case 0:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 1:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 2:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 3:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 4:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 5:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 6:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
          case 7:
          {
            if ((ADC_BUF[i] > 111) && (ADC_BUF[i] < 113))
              maze.Arm[i].Variability.delta++;
            if (maze.Arm[i].Variability.delta >= 10000)
              maze.State = RAT_NOT_ENTERED;
            break;
          }
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

/************************ flawless0714 *****END OF FILE****/
