#include "main.h"

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

void TM_SystemInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; /* Alternate Function I/O clock enabled */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* Power interface clock enable */
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	RCC->CR |= 1<<0; /* enable HSI clock */
	while((RCC->CR & (uint32_t)(1<<1)) == 0);
	RCC->APB2ENR |= (uint32_t)(1<<2);/* I/O port A clock enable */
	RCC->APB2ENR |= (uint32_t)(1<<3);/* I/O port B clock enable */
	RCC->APB2ENR |= (uint32_t)(1<<4);/* I/O port C clock enable */
	RCC->APB2ENR |= (uint32_t)(1<<5);/* I/O port C clock enable */
	RCC->APB2ENR |= (uint32_t)(1<<11);/* TIM1 */
}

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

void TM_EXTI_Init(void)
{
	AFIO->EXTICR[2] |= 1UL<<8U;
	EXTI->IMR |= 1UL<<10U; /* Interrupt request from Line x is not masked */
	EXTI->EMR |= 1UL<<10U; /* Event request from Line x is not masked */
	EXTI->FTSR |= 1UL<<10U; /* Falling trigger enabled (for Event and Interrupt) for input line. */
	//__NVIC_SetPriority(EXTI15_10_IRQn, 0);
	__NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void TM_TIM_Init()
{
	TIM1->CR1 = 0;
	TIM1->DIER |= 1UL<<0U;
	TIM1->EGR |= 1UL<<0U;
	TIM1->ARR = 79;
	TIM1->PSC = 599;
	TIM1->CNT = 0;
	__NVIC_EnableIRQ(TIM1_UP_IRQn);
	TIM1->CR1 |= 1;
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

void TIM1_UP_IRQHandler(void)
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

void delay(uint32_t time)
{
	while(time--);
}

int main()
{
	TM_SystemInit();
	TM_GPIO_Init();
	TM_EXTI_Init();
	TM_TIM_Init();
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_1;
	while(1)
	{
		GPIOC->BSRR |= 3UL<<0U;
		delay(720000);
		GPIOC->BRR |= 3UL<<0U;
		delay(720000);
		num++; if(num>9999)num=0;
	}
	return 0;
}
