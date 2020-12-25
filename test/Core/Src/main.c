#include "stm32f1xx.h"
#include <stdio.h>

uint8_t index;
#define LIGHT (1UL<<5U)
#define AnhSang_PORT GPIOA
void TM_SystemInit(void)
{
	RCC->CR |= 1ul<<0u; /* enable HSI clock */
	while((RCC->CR & (uint32_t)(1ul<<1u)) == 0);
	RCC->APB2ENR |= (uint32_t)(1<<2);/* I/O port A clock enable */
	RCC->APB2ENR |= (uint32_t)(1<<3);/* I/O port B clock enable */
	RCC->APB2ENR |= (uint32_t)(1ul<<4u);/* I/O port C clock enable */
	RCC->APB2ENR |= (uint32_t)(1<<5);/* I/O port D clock enable */
	RCC->APB2ENR |= (uint32_t)(1ul<<11u);/* TIM1 */
	RCC->APB2ENR |= (uint32_t)(1ul<<9u); //ADC1
}
void TM_GPIO_Init(void)
{
	GPIOC->CRL |= 3UL<<0U;/* MODE */
	GPIOC->CRL &= ~(3UL<<2U); /*CNF*/
	GPIOC->CRL |= 3UL<<4U;/* MODE */
	GPIOC->CRL &= ~(3UL<<6U); /*CNF*/
	GPIOA->CRL |= 3UL<<20U;/* MODE */
	GPIOA->CRL &= ~(3UL<<22U); /*CNF*/
}
void TM_ADC_Init(void){
	ADC1->SR |= 1ul<<4U; //STRT start regular channels
	ADC1->CR1 |=1ul<<23U; //Analog watchdog enable on regular channels
}

void TM_TIM_Init(void)
{
	TIM1->DIER |= 1UL<<0U;
	TIM1->EGR |= 1UL<<0U;
	TIM1->ARR = 799;
	TIM1->PSC = 9999;
	TIM1->CNT = 0;
	TIM1->CR1 |= 1ul<<0;
	__NVIC_EnableIRQ(TIM1_UP_IRQn);
}

void TIM1_UP_IRQHandler(void)
{
	/*if(index==1){
		index=0;
	}else{
		index = 1;
	}
	if(index==0){
		GPIOC->BSRR |= 1UL<<1U;
	}else{
		GPIOC->BRR |= 1UL<<1U;
	}
	TIM1->SR &= ~(1UL<<0U);*/
}

void delay(uint32_t time){
	while(time--);
}

int main()
{
	TM_SystemInit();
	TM_GPIO_Init();
	TM_TIM_Init();
	TM_ADC_Init();
	while(1)
	{
		if(ADC1->DR == 0x0000FFFF){
			GPIOC->BSRR |= 1UL<<0U;
			delay(72000);
			GPIOC->BRR |= 1UL<<0U;
			delay(72000);
		}
	}
 return 0;
}
