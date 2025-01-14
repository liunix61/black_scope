/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "scope_tasks.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for taskTsc */
osThreadId_t taskTscHandle;
const osThreadAttr_t taskTsc_attributes = {
  .name = "taskTsc",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 256 * 4
};
/* Definitions for taskUi */
osThreadId_t taskUiHandle;
const osThreadAttr_t taskUi_attributes = {
  .name = "taskUi",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4 + 128 *2
};
/* Definitions for taskScope */
osThreadId_t taskScopeHandle;
const osThreadAttr_t taskScope_attributes = {
  .name = "taskScope",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 128 * 6 + 128
};
/* Definitions for taskWavegen */
osThreadId_t taskWavegenHandle;
const osThreadAttr_t taskWavegen_attributes = {
  .name = "taskWavegen",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 128 * 4
};
/* Definitions for queueTscUi */
osMessageQueueId_t queueTscUiHandle;
const osMessageQueueAttr_t queueTscUi_attributes = {
  .name = "queueTscUi"
};
/* Definitions for queueUiLcd */
osMessageQueueId_t queueUiLcdHandle;
const osMessageQueueAttr_t queueUiLcd_attributes = {
  .name = "queueUiLcd"
};
/* Definitions for queueUiScope */
osMessageQueueId_t queueUiScopeHandle;
const osMessageQueueAttr_t queueUiScope_attributes = {
  .name = "queueUiScope"
};
/* Definitions for queueUiWavegen */
osMessageQueueId_t queueUiWavegenHandle;
const osMessageQueueAttr_t queueUiWavegen_attributes = {
  .name = "queueUiWavegen"
};
/* Definitions for semaphoreLcd */
osSemaphoreId_t semaphoreLcdHandle;
const osSemaphoreAttr_t semaphoreLcd_attributes = {
  .name = "semaphoreLcd"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/*struct sQueueTscUi {
    uint16_t p;
    uint16_t x;
    uint16_t y;
};
osMessageQueueId_t queueTscUiHandle;
const osMessageQueueAttr_t queueTscUi_attributes = {
  .name = "queueTscUi"
};

struct sQueueUiLcd {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t *buf;
};

osMessageQueueId_t queueUiLcdHandle;
const osMessageQueueAttr_t queueUiLcd_attributes = {
  .name = "queueUiLcd"
};

// declare and initialize the semaphoreLcdHandle
osSemaphoreId_t semaphoreLcdHandle;
const osSemaphoreAttr_t semaphoreLcd_attributes = {
  .name = "semaphoreLcd"
};

struct sQueueUiScope {
    uint16_t type;
    uint16_t data[8];
};

osMessageQueueId_t queueUiScopeHandle;
const osMessageQueueAttr_t queueUiScope_attributes = {
  .name = "queueUiScope"
};


struct sQueueUiWavegen {
    uint16_t type;
    uint16_t data[8];
};

osMessageQueueId_t queueUiWavegenHandle;
const osMessageQueueAttr_t queueUiWavegen_attributes = {
  .name = "queueUiWavegen"
};*/

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskTsc(void *argument);
extern void StartTaskUi(void *argument);
extern void StartTaskScope(void *argument);
extern void StartTaskWavegen(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
extern void ConfigureRuntimeStatsTimer();
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	//ConfigureRuntimeStatsTimer();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of semaphoreLcd */
  //semaphoreLcdHandle = osSemaphoreNew(1, 1, &semaphoreLcd_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	semaphoreLcdHandle = osSemaphoreNew(1, 0, &semaphoreLcd_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of queueTscUi */
  queueTscUiHandle = osMessageQueueNew (1, sizeof(struct sQueueTscUi), &queueTscUi_attributes);

  /* creation of queueUiLcd */
  //queueUiLcdHandle = osMessageQueueNew (4, sizeof(struct sQueueUiLcd), &queueUiLcd_attributes);

  /* creation of queueUiScope */
  queueUiScopeHandle = osMessageQueueNew (4, sizeof(struct sQueueUiScope), &queueUiScope_attributes);

  /* creation of queueUiWavegen */
  queueUiWavegenHandle = osMessageQueueNew (4, sizeof(struct sQueueUiWavegen), &queueUiWavegen_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
	//queueTscUiHandle = osMessageQueueNew(1, sizeof(struct sQueueTscUi), &queueTscUi_attributes );
	//queueUiLcdHandle = osMessageQueueNew(1, sizeof(struct sQueueUiLcd), &queueUiLcd_attributes );
	//queueUiScopeHandle = osMessageQueueNew(1, sizeof(struct sQueueUiScope), &queueUiScope_attributes );
	//queueUiWavegenHandle = osMessageQueueNew(1, sizeof(struct sQueueUiWavegen), &queueUiWavegen_attributes );
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of taskTsc */
  taskTscHandle = osThreadNew(StartTaskTsc, NULL, &taskTsc_attributes);

  /* creation of taskUi */
  taskUiHandle = osThreadNew(StartTaskUi, NULL, &taskUi_attributes);

  /* creation of taskScope */
  taskScopeHandle = osThreadNew(StartTaskScope, NULL, &taskScope_attributes);

  /* creation of taskWavegen */
  taskWavegenHandle = osThreadNew(StartTaskWavegen, NULL, &taskWavegen_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
//#include "lcd.h"
//extern tLcd lcd;
//uint8_t lcd_buf[480*2];
//#include "framebuf.h"
//extern tFramebuf fb;

// 'BBBhh', type, button, down, x, y
struct sUsbMsg{
	uint8_t type;
	uint8_t button;
	uint8_t down;
	int16_t x;
	int16_t y;
};
typedef struct sUsbMsg tUsbMsg;
tUsbMsg usb_msg;
uint8_t usb_buf[32];
uint8_t usb_buf_flag = 0;
#include "nuklear.h"
#define RECTS_LEN 64
extern struct nk_rect rects[RECTS_LEN];
extern int rects_max;
extern int rects_ptr;
extern int rects_pressed;


#include "cmsis_os.h"
#include "lcd.h"
#include "scope_tasks.h"
#include "font.h"
#include "FontUbuntuBookRNormal16.h"

int fsm_state = 0;

int is_value( struct nk_rect rect )
{
	if( rect.x == 10 &&
		rect.w == 20 &&
		rect.h == 30 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int is_scroll( struct nk_rect rect )
{
	if( rect.x == 10 &&
		rect.w == 20 &&
		rect.h == 30 )
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
#define FSM_STATE_DEFAULT 	0
#define FSM_STATE_ON_VALUE 	1
#define FSM_STATE_ON_SCROLL 2
#include <string.h>
void fsm_process()
{
	struct sQueueTscUi msg = {0};

	switch( fsm_state )
	{
		case FSM_STATE_DEFAULT:
		{
			if( usb_buf_flag )
			{
				memcpy( &usb_msg, usb_buf, sizeof( usb_msg ) );
				usb_buf_flag = 0;
				switch( usb_msg.type )
				{
					case 0: // press
						if( is_value( rects[rects_ptr] ) )
						{
							if( usb_msg.button == 3 )
							{
								fsm_state = FSM_STATE_ON_VALUE;
							}
						}
						else if( is_scroll( rects[rects_ptr] ) )
						{
							if( usb_msg.button == 3 )
							{
								fsm_state = FSM_STATE_ON_SCROLL;
							}
						}
						else
						{
							if( usb_msg.button == 3 )
							{
								rects_pressed = 1;
								msg.p = 1;
								msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
								msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
								osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
							}
						}
						break;
					case 1: // release
						if( is_value( rects[rects_ptr] ) )
						{
							if( usb_msg.button == 3 )
							{
								fsm_state = FSM_STATE_ON_VALUE;
							}
						}
						else if( is_scroll( rects[rects_ptr] ) )
						{
							if( usb_msg.button == 3 )
							{
								fsm_state = FSM_STATE_ON_SCROLL;
							}
						}
						else
						{
							if( usb_msg.button == 3 )
							{
								rects_pressed = 0;
								msg.p = 0;
								msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
								msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
								osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
							}
						}
						break;
					case 2: // wheel
					{
						if( usb_msg.y == 120 )
						{
							if( rects_ptr > 0 )
							{
								rects_ptr -= 1;
							}
							else
							{
								rects_ptr = rects_max-1;
							}
						}
						else// if( usb_msg.y > 0 )
						{
				    		if( rects_ptr < rects_max )
				    		{
				    			rects_ptr += 1;
				    		}
				    		else
				    		{
				        		rects_ptr = 0;
				    		}
						}

						rects_pressed = 0;
						msg.p = 0;
						msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
						msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
						osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
						break;
					}
				}
			}
			break;
		}
		case FSM_STATE_ON_VALUE:
		{
			if( usb_buf_flag )
			{
				memcpy( &usb_msg, usb_buf, sizeof( usb_msg ) );
				usb_buf_flag = 0;
				switch( usb_msg.type )
				{
					case 0: // press
						//fsm_state = FSM_STATE_DEFAULT;
						break;
					case 1: // release
						fsm_state = FSM_STATE_DEFAULT;
						break;
					case 2: // wheel
					{
						rects_pressed = 0;
						msg.p = 0;
						msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
						msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
						osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
						break;
					}
				}
			}
			break;
		}
		case FSM_STATE_ON_SCROLL:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}
void StartDefaultTask(void *argument)
{
	extern int ui_menu;
	struct sQueueTscUi msg = {0};
	while(1)
	{
		osDelay(50);
		if( usb_buf_flag ){
			memcpy( &usb_msg, usb_buf, sizeof( usb_msg ) );
			usb_buf_flag = 0;
			switch( usb_msg.type )
			{
				case 0: // BTN1
				{
					ui_menu = 1;
					rects_pressed = 0;
					msg.p = 0;
					msg.x = 0;
					msg.y = 0;
					//osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}
				case 1: // BTN2
				{
					ui_menu = 2;
					rects_pressed = 0;
					msg.p = 0;
					msg.x = 0;
					msg.y = 0;
					//osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}
				case 2: // BTN3
				{
					ui_menu = 3;
					rects_pressed = 0;
					msg.p = 0;
					msg.x = 0;
					msg.y = 0;
					//osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}
				case 3: // BTN4
				{
					ui_menu = 4;
					rects_pressed = 0;
					msg.p = 0;
					msg.x = 0;
					msg.y = 0;
					//osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}

				case 4: // INC
				{
					if( rects_ptr < rects_max )
					{
						rects_ptr += 1;
					}
					else
					{
						rects_ptr = 0;
					}
					rects_pressed = 0;
					msg.p = 0;
					msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
					msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
					osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}
				case 5: // DEC
				{
					if( rects_ptr > 0 )
					{
						rects_ptr -= 1;
					}
					else
					{
						rects_ptr = rects_max-1;
					}
					rects_pressed = 0;
					msg.p = 0;
					msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
					msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
					osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}
				case 6: // PRESS
				{
					rects_pressed = 1;
					msg.p = 1;
					msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
					msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
					osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);

					rects_pressed = 0;
					msg.p = 0;
					msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
					msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
					osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
					break;
				}
				default:
					break;
			}

		}
	}
}
void _StartDefaultTask(void *argument)
{
	int i, j;
	osDelay(100);
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	struct sQueueTscUi msg = {0};
  for(;;)
  {

	if( usb_buf_flag ){
		memcpy( &usb_msg, usb_buf, sizeof( usb_msg ) );
		//msg.p = 1;
		//msg.x = rects[rects_ptr].x+240 + 10;
		//msg.y = rects[rects_ptr].y + 10;
		//osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);

		switch( usb_msg.type )
		{
		case 0: // press
			if( usb_msg.button == 3 )
			{
				rects_pressed = 1;
				msg.p = 1;
				msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
				msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
				osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
			}
			break;
		case 1: // release
			if( usb_msg.button == 3 )
			{
				rects_pressed = 0;
				msg.p = 0;
				msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
				msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
				osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
			}
			break;
		case 2: // wheel
			if( usb_msg.y == 120 )
			{
				if( rects_ptr > 0 )
				{
					rects_ptr -= 1;
				}
				else
				{
					rects_ptr = rects_max-1;
				}
			}
			else// if( usb_msg.y > 0 )
			{
	    		if( rects_ptr < rects_max )
	    		{
	    			rects_ptr += 1;
	    		}
	    		else
	    		{
	        		rects_ptr = 0;
	    		}
			}

			{
				rects_pressed = 0;
				msg.p = 0;
				msg.x = rects[rects_ptr].x + rects[rects_ptr].w/2 + 240;
				msg.y = rects[rects_ptr].y + rects[rects_ptr].h/2;
				osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
			}

			/*struct nk_rect *bounds;
			bounds = &rects[rects_ptr];
			extern tLcd lcd;
		    if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
		    {
		    	if( 0 <= (int)bounds->y && (int)bounds->y < 320 && 0 <= (int)bounds->y+(int)bounds->h && (int)bounds->y+(int)bounds->h < 320 )
		    	{
					lcd_hline( &lcd, (int)bounds->x, (int)bounds->y, (int)bounds->w, LCD_COLOR_GREEN );
					lcd_hline( &lcd, (int)bounds->x, (int)bounds->y+(int)bounds->h, (int)bounds->w, LCD_COLOR_GREEN );
					lcd_vline( &lcd, (int)bounds->x, (int)bounds->y, (int)bounds->h, LCD_COLOR_GREEN );
					lcd_vline( &lcd, (int)bounds->x+(int)bounds->w, (int)bounds->y, (int)bounds->h, LCD_COLOR_GREEN );
					uint8_t buffer[32];
					snprintf( buffer, sizeof(buffer), "%d", rects_ptr );
					font_draw_text( &fontUbuntuBookRNormal16, (int)bounds->x, (int)bounds->y, buffer, LCD_COLOR_GREEN, lcd_set_pixel, &lcd );
					if( rects_ptr == rects_max )
					{
						lcd_rect( &lcd, (int)bounds->x, (int)bounds->y, 5, 5, LCD_COLOR_GREEN );
					}

					//rects[rects_max] = *bounds;
					//rects_max += 1;
		    	}
		        osSemaphoreRelease( semaphoreLcdHandle );
		    }*/

			break;

		}
		usb_buf_flag = 0;

	}

	  /*for( i = 0 ; i < 80 ; i++ )
	  {
	      if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	      {
			  for( j = 0 ; j < 240 ; j++ )
			  {
				  lcd_buf[j] = framebuf_get_pixel( &fb, j, i );
			  }
	      //    lcd_get_bmp( &lcd, 0, i, 480, 1, lcd_buf );
	          osSemaphoreRelease( semaphoreLcdHandle );
	      }
	      lcd_buf[0] = i;
	      lcd_buf[1] = 0;
	      extern uint8_t usb_done;
	      usb_done = 0;
	      while( 1 == CDC_Transmit_FS( lcd_buf, 480*2 ) )
	      {
	    	  osDelay(1);
	      }
	      while( !usb_done )
	      {
	    	  osDelay(1);
	      }
	  }*/

      osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskTsc */
/**
* @brief Function implementing the taskTsc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskTsc */
void _StartTaskTsc(void *argument)
{
  /* USER CODE BEGIN StartTaskTsc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskTsc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void ConfigureRuntimeStatsTimer() {
    TIM_HandleTypeDef htim8;

    // Enable TIM8 clock
    __HAL_RCC_TIM8_CLK_ENABLE();

    // Initialize TIM8 handle
    htim8.Instance = TIM8;
    htim8.Init.Prescaler = (uint32_t)(SystemCoreClock / 1000) - 1;  // 1 kHz timer frequency
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 999;  // 1 ms period
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&htim8) != HAL_OK) {
        // Initialization Error
        Error_Handler();  // Implement your error handling mechanism here
    }

    // Start the TIM8 base timer in interrupt mode
    if (HAL_TIM_Base_Start_IT(&htim8) != HAL_OK) {
        // Start Error
        Error_Handler();  // Implement your error handling mechanism here
    }

    // Enable TIM8 interrupt in NVIC
   // HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
  //  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
}
// TIM8 interrupt handler
void _TIM8_UP_TIM13_IRQHandler() {
    // Clear timer interrupt flag
   // __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_UPDATE);

    // Update FreeRTOS tick count
    //vTaskStepTick();
}
uint32_t get_tim8_cnt()
{
	return TIM8->CNT;
}
/* USER CODE END Application */

