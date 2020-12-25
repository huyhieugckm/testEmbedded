#include "main.h"
#include "cmsis_os.h"

#define HC595_DATA_PIN	(1UL<<5U)/* PB5 */
#define HC595_CLK_PIN	(1UL<<3U)/* PB3 */
#define HC595_LAT_PIN	(1UL<<2U)/* PD2 */

#define HC595_DATA_PORT GPIOB
#define HC595_CLK_PORT 	GPIOB
#define HC595_LAT_PORT 	GPIOD


uint8_t led_status = 0;

uint8_t index;
uint32_t num = 0;

uint8_t LED[11] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

osThreadId_t MyTask01Handle;
const osThreadAttr_t MyTask01_attributes = {
  .name = "MyTask01",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for MyTask02 */
osThreadId_t MyTask02Handle;
const osThreadAttr_t MyTask02_attributes = {
  .name = "MyTask02",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void StartMyTask01(void *argument);
void StartMyTask02(void *argument);

void TM_GPIO_Init(void)
{
	GPIOC->CRL |= 3UL<<0U;/* MODE */
	GPIOC->CRL &= ~(3UL<<2U); /*CNF*/
	GPIOC->CRL |= 3UL<<4U;/* MODE */
	GPIOC->CRL &= ~(3UL<<6U); /*CNF*/

	/*PB5*/
	GPIOB->CRL |= 3UL<<20U;/* MODE */
	GPIOB->CRL &= ~(3UL<<22U); /*CNF*/

	/*PB3*/
	GPIOB->CRL |= 3UL<<12U;/* MODE */
	GPIOB->CRL &= ~(3UL<<14U); /*CNF*/

	/*PD2*/
	GPIOD->CRL |= 3UL<<8U;/* MODE */
	GPIOD->CRL &= ~(3UL<<10U); /*CNF*/


	GPIOB->CRH &= ~(3UL<<10U);
	GPIOB->CRH |= (2UL<<10U); /* Input mode with pull-up/ pull-down */
	GPIOB->ODR |= (1UL<<10U); /* pull-up */
}

void HC595_Write(uint8_t data)
{
	for(int i=0; i<8; i++)
	{
		if(((data<<i)&0x80) == 0x80)
		{
			HC595_DATA_PORT->BSRR |= HC595_DATA_PIN; /* 1 */
		}
		else
		{
			HC595_DATA_PORT->BRR |= HC595_DATA_PIN; /* 0 */
		}
		HC595_CLK_PORT->BSRR |= HC595_CLK_PIN; /* 1 */
		HC595_CLK_PORT->BRR |= HC595_CLK_PIN; /* 0 */
	}
}

void OutLed(void)
{
	HC595_LAT_PORT->BSRR |= HC595_LAT_PIN; /* 1 */
	HC595_LAT_PORT->BRR |= HC595_LAT_PIN; /* 0 */
}

int main(void)
{
  TM_GPIO_Init();
  //HAL_Init();
  SystemClock_Config();
  osKernelInitialize();

  MyTask01Handle = osThreadNew(StartMyTask01, NULL, &MyTask01_attributes);

  /* creation of MyTask02 */
  MyTask02Handle = osThreadNew(StartMyTask02, NULL, &MyTask02_attributes);

  osKernelStart();

  while (1)
  {

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void StartMyTask01(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  switch(index)
	  	{
	  		case 0:
	  			HC595_Write(LED[num/1000]);
	  			HC595_Write(0x10);
	  			OutLed();
	  			break;
	  		case 1:
	  			HC595_Write(LED[(num%1000)/100]);
	  			HC595_Write(0x20);
	  			OutLed();
	  			break;
	  		case 2:
	  			HC595_Write(LED[((num%1000)%100)/10]);
	  			HC595_Write(0x40);
	  			OutLed();
	  			break;
	  		case 3:
	  			HC595_Write(LED[((num%1000)%100)%10]);
	  			HC595_Write(0x80);
	  			OutLed();
	  			break;
	  	}
	  	index++;if(index>3)index=0;
	  	TIM1->SR &= ~(1UL<<0U);
  }
  /* USER CODE END 5 */
}

void StartMyTask02(void *argument)
{
  /* USER CODE BEGIN StartMyTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartMyTask02 */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
