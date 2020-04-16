/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "spi.h"
#include "SEGGER_SYSVIEW.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

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
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
	  SEGGER_SYSVIEW_Conf();

  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
  //SEGGER_SYSVIEW_Start();
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
//GENERATE LUT
static uint8_t RGB_SPI_LUT[256*3]={0};
const uint32_t mask=0b000000000100100100100100100100100; //Mask for low and high mask is ID7O ID6O ID5O ID4O ID3O ID2O ID1O ID0O
for(uint32_t i=0;i<256;i++)
{
	uint32_t tmp=mask;
	uint32_t DataPos3=0;
	//loop over evry bit in the data byte
	for(uint8_t j=0;j<8;j++){
		uint32_t DataBit=i&(1<<j);
		if(DataBit>0){
			DataPos3=1<<(3*j+1);// set bit at pos 3j+1 to 1 this is the pos Dj in  ID7O ID6O ID5O ID4O ID3O ID2O ID1O ID0O
		}
		else
		{
			DataPos3=0;
		}
		tmp=tmp|DataPos3;
	}
	RGB_SPI_LUT[i*3+2]=tmp; //LSB here send last
	RGB_SPI_LUT[i*3+1]=tmp>>8;
	RGB_SPI_LUT[i*3+0]=tmp>>16; //MSB here send first
}

//LED ARRAY
#define NUMLED 200
#define LENLEDDATA NUMLED*3
static uint8_t LEDData[LENLEDDATA]={0};
for (int i=0;i<NUMLED;i++)
{
	LEDData[i*3]=(uint8_t) i;
	LEDData[i*3+1]=(uint8_t)i*2;
	LEDData[i*3+2]=(uint8_t) i*3;
}
static uint8_t outputdata[17+LENLEDDATA*3+1]={0};
outputdata[0]=0xFF;
#define LENOUTPTDATA 17+LENLEDDATA*3+1
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		osDelay(150);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		unsigned char* srcPtr = LEDData;
		unsigned char* dstPtr = &outputdata[17];//using offet indexing for rest peramble
		for (int i = 0; i < LENLEDDATA; i++)
		       {
		        unsigned char ucVal = *srcPtr;
		        srcPtr++;
		        int lutOff = ucVal * 3;
		        unsigned char* ptrLut = RGB_SPI_LUT + lutOff;
                dstPtr[0] = ptrLut[0];
                dstPtr[1] = ptrLut[1];
                dstPtr[2] = ptrLut[2];
		        dstPtr += 3;

		       }
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_SPI_Transmit_DMA(&hspi1,&outputdata,LENOUTPTDATA);

  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
